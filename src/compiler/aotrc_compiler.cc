//
// Created by charlie on 5/11/22.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <fstream>
#include <iostream>
#include "aotrc_compiler.h"
#include "regex_compiler.h"

aotrc::compiler::AotrcCompiler::AotrcCompiler()
: llvmContext() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Initialize the compilation target
    std::string error;
    this->target = llvm::TargetRegistry::lookupTarget(LLVM_DEFAULT_TARGET_TRIPLE, error);
    if (!this->target)
        throw std::runtime_error(error);

    llvm::TargetOptions options;
    this->targetMachine = std::unique_ptr<llvm::TargetMachine>(
            target->createTargetMachine(LLVM_DEFAULT_TARGET_TRIPLE, "generic", "", options, llvm::Optional<llvm::Reloc::Model>())
    );
}


void aotrc::compiler::AotrcCompiler::compileRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA) {

    // Create a new module if necessary
    if (this->modules.find(module) == this->modules.end()) {
        this->modules[module] = std::make_unique<llvm::Module>(module, this->llvmContext);
    }
    std::unique_ptr<llvm::Module> &mod = this->modules.at(module);

    // Get the appropriate regex compiler
    FullMatchProgramCompiler fullMatchCompiler(this->llvmContext);
    fullMatchCompiler.compile(mod, label, regexDFA);
}


void aotrc::compiler::AotrcCompiler::compileSubMatchRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA) {
    // Create a new module if necessary
    if (this->modules.find(module) == this->modules.end()) {
        this->modules[module] = std::make_unique<llvm::Module>(module, this->llvmContext);
    }
    std::unique_ptr<llvm::Module> &mod = this->modules.at(module);

    // Get the appropriate regex compiler
    SubMatchProgramCompiler subMatchProgramCompiler(this->llvmContext);
    subMatchProgramCompiler.compile(mod, label, regexDFA);
}


void aotrc::compiler::AotrcCompiler::compileSearchRegex(const std::string &module, const std::string &label,
                                                        const aotrc::fa::DFA &regexDFA) {
    // Create a new module if necessary
    if (this->modules.find(module) == this->modules.end()) {
        this->modules[module] = std::make_unique<llvm::Module>(module, this->llvmContext);
    }
    std::unique_ptr<llvm::Module> &mod = this->modules.at(module);

    // Get the appropriate regex compiler
    SearchProgramCompiler searchProgramCompiler(this->llvmContext);
    searchProgramCompiler.compile(mod, label, regexDFA);
}


void aotrc::compiler::AotrcCompiler::compileCaptureRegex(const std::string &module, const std::string &label,
                                                         const aotrc::fa::DFA &regexDFA) {

    // Create a new module if necessary
    if (this->modules.find(module) == this->modules.end()) {
        this->modules[module] = std::make_unique<llvm::Module>(module, this->llvmContext);
    }
    std::unique_ptr<llvm::Module> &mod = this->modules.at(module);

    std::vector<std::string> groupNames;
    for (unsigned int i = 1; i <= regexDFA.getGroupCount(); i++) {
        groupNames.push_back("group" + std::to_string(i));
    }

    // Create the group info
    CaptureInfoBuilder infoBuilder(this->llvmContext, label, groupNames);

    // Get the appropriate regex compiler
    CaptureProgramCompiler captureProgramCompiler(this->llvmContext, infoBuilder.buildStruct());
    captureProgramCompiler.compile(mod, label, regexDFA);
}


std::string aotrc::compiler::AotrcCompiler::emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type) {
    auto &mod = this->modules.at(module);
    mod->setDataLayout(this->targetMachine->createDataLayout());
    mod->setCodeModel(this->targetMachine->getCodeModel());

    std::error_code err;
    llvm::raw_fd_ostream output(outputPath, err);
    if (err) {
        // llvm::errs() << "Could not open file: " << err.message();
        throw std::runtime_error(err.message());
    }

    // Add a pass to generate machine code
    llvm::legacy::PassManager passManager;
    this->targetMachine->setOptLevel(llvm::CodeGenOpt::Level::Default); // Aggressive code optimization?
    if (this->targetMachine->addPassesToEmitFile(passManager, output, nullptr, type)) {
        llvm::errs() << "Error while emitting to file";
    }

    // Run all the passes
    passManager.run(*mod);
    output.flush();

    return outputPath;
}

std::string aotrc::compiler::AotrcCompiler::emitIr(const std::string &module) {
    auto &mod = this->modules.at(module);
    std::string output;
    llvm::raw_string_ostream os(output);
    os << *mod;
    std::cout << output << std::endl;
    return "";
}

std::string aotrc::compiler::AotrcCompiler::emitAssembly(const std::string &module, const std::string &outputPath) {
    return this->emitCode(module, outputPath, llvm::CGFT_AssemblyFile);
}

std::string aotrc::compiler::AotrcCompiler::emitAssembly(const std::string &module) {
    std::string defaultName = module + ".s";
    return this->emitAssembly(module, defaultName);
}

std::string aotrc::compiler::AotrcCompiler::emitObjectFile(const std::string &module, const std::string &outputPath) {
    return this->emitCode(module, outputPath, llvm::CGFT_ObjectFile);
}


std::string aotrc::compiler::AotrcCompiler::emitObjectFile(const std::string &module) {
    std::string defaultName = module + ".o";
    return this->emitObjectFile(module, defaultName);
}

static llvm::GlobalVariable *
build_global(const std::string &label, const std::string &pattern,
             std::unique_ptr<llvm::Module> &parent, llvm::LLVMContext &ctx) {

    // Create a global character array that holds the pattern
    std::string uppercaseLabel = label;
    std::transform(uppercaseLabel.begin(),  uppercaseLabel.end(), uppercaseLabel.begin(), ::toupper);
    std::string globalVarName = uppercaseLabel + "_PATTERN";
    auto charArrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx), pattern.length() + 1);
    parent->getOrInsertGlobal(globalVarName, charArrayType);
    auto patternGlobal = parent->getGlobalVariable(globalVarName);

    // Create a constant array out of the pattern
    std::vector<llvm::Constant *> patternCharacterConstants;
    auto charType = llvm::Type::getInt8Ty(ctx);
    for (const auto &c : pattern) {
        auto literalChar = llvm::ConstantInt::get(charType, (long) c);
        patternCharacterConstants.push_back(literalChar);
    }
    // Add a null terminator
    patternCharacterConstants.push_back(llvm::ConstantInt::get(charType, (long) 0));

    // Set a constant initializer
    patternGlobal->setInitializer(llvm::ConstantArray::get(charArrayType, patternCharacterConstants));

    return patternGlobal;
}

void aotrc::compiler::AotrcCompiler::generatePatternFunc(const std::string &module, const std::string &label,
                                                    const std::string &regex) {

    // Create an IR builder
    llvm::IRBuilder builder(this->llvmContext);

    // Get the module
    auto &mod = this->modules.at(module);

    // Build a global that holds the array
    auto patternGlobal = build_global(label, regex, mod, this->llvmContext);

    // Build a function that returns said array
    auto charPtrType = llvm::Type::getInt8Ty(this->llvmContext)->getPointerTo();
    auto getPatternFunctionType = llvm::FunctionType::get(charPtrType, false);
    std::string functionName = label + "_get_pattern";
    mod->getOrInsertFunction(functionName, getPatternFunctionType);
    auto getFunc = mod->getFunction(functionName);

    // Return a pointer to the global?
    auto mainBB = llvm::BasicBlock::Create(this->llvmContext, "main", getFunc);
    builder.SetInsertPoint(mainBB);

    // Get pointer to the global
    auto ptr = builder.CreateGEP(charPtrType, patternGlobal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(this->llvmContext), 0), "pointerToFirst");
    builder.CreateRet(ptr);
}

std::string aotrc::compiler::AotrcCompiler::emitHeaderFile(const std::string &module, const std::string &outputPath) {
    auto &mod = this->modules[module];

    std::ofstream headerOutput(outputPath);
    // Emit C guard and c++ declaration
    headerOutput << "#ifndef _AOTRC_" << module << "_H\n";
    headerOutput << "#define _AOTRC_" << module << "\n\n";
    headerOutput << "#ifdef __cplusplus\n";
    headerOutput << "extern \"C\" {\n";
    headerOutput << "#endif\n\n";
    headerOutput << "// This file was auto generated by aotrc\n";

    // TODO this probably isn't very portable
    headerOutput << "#include <stdbool.h>\n\n";

    // For each function, output a function definition
    for (const auto &func : mod->functions()) {
        headerOutput << llvmTypeToCType(llvmContext, func.getReturnType()) << ' ' << func.getName().str() << '(';
        // comma separated list of all the parameters
        if (!func.arg_empty()) {
            auto argIt = func.arg_begin();
            for (; argIt != func.arg_end() - 1; ++argIt) {
                headerOutput << llvmTypeToCType(llvmContext, argIt->getType());
                if (argIt->hasName()) {
                    headerOutput << argIt->getName().str();
                }
                headerOutput << ", ";
            }
            // Do the last arg
            headerOutput << llvmTypeToCType(llvmContext, argIt->getType());
            if (argIt->hasName()) {
                headerOutput << argIt->getName().str();
            }
        }

        // Close the function def
        headerOutput << ");\n";
    }
    headerOutput << '\n';

    // Emit C closing guard
    headerOutput << "#ifdef __cplusplus\n";
    headerOutput << "}\n";
    headerOutput << "#endif\n";
    headerOutput << "#endif\n";

    // Flush
    headerOutput.flush();

    return outputPath;
}

std::string aotrc::compiler::AotrcCompiler::emitHeaderFile(const std::string &module) {
    std::string defaultName = module + ".h";
    return this->emitHeaderFile(module, defaultName);
}
