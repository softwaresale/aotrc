//
// Created by charlie on 3/13/22.
//

#include "compiler.h"

#include <iostream>
#include <fstream>
#include "program.h"
#include "src/compiler/passes/restart_pass.h"
#include "search_program.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>

aotrc::compiler::Compiler::Compiler()
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

bool aotrc::compiler::Compiler::compileRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc) {
    return this->compileProgram<aotrc::compiler::FullMatchProgram>(module, label, regex, genPatternFunc);
}

bool aotrc::compiler::Compiler::compileSubmatchRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc) {
    return this->compileProgram<aotrc::compiler::SearchProgram>(module, label, regex, genPatternFunc);
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

bool aotrc::compiler::Compiler::generatePatternFunc(const std::string &module, const std::string &label,
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

    return true;
};


std::string aotrc::compiler::Compiler::emitIr(const std::string &module) {
    auto &mod = this->modules.at(module);
    std::string output;
    llvm::raw_string_ostream os(output);
    os << *mod;
    std::cout << output << std::endl;
    return "";
}

std::string aotrc::compiler::Compiler::emitAssembly(const std::string &module, const std::string &outputPath) {
    return this->emitCode(module, outputPath, llvm::CGFT_AssemblyFile);
}

std::string aotrc::compiler::Compiler::emitAssembly(const std::string &module) {
    std::string defaultName = module + ".s";
    return this->emitAssembly(module, defaultName);
}

std::string aotrc::compiler::Compiler::emitObjectFile(const std::string &module, const std::string &outputPath) {
    return this->emitCode(module, outputPath, llvm::CGFT_ObjectFile);
}


std::string aotrc::compiler::Compiler::emitObjectFile(const std::string &module) {
    std::string defaultName = module + ".o";
    return this->emitObjectFile(module, defaultName);
}


std::string aotrc::compiler::Compiler::emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type) {
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

std::string aotrc::compiler::Compiler::llvmTypeToCType(llvm::Type *type) {
    static auto boolWidth = llvm::IntegerType::getInt1Ty(this->llvmContext)->getBitWidth();
    static auto charWidth = llvm::IntegerType::getInt8Ty(this->llvmContext)->getBitWidth();
    static auto shortWidth = llvm::IntegerType::getInt16Ty(this->llvmContext)->getBitWidth();
    static auto intWidth = llvm::IntegerType::getInt32Ty(this->llvmContext)->getBitWidth();
    static auto longWidth = llvm::IntegerType::getInt64Ty(this->llvmContext)->getBitWidth();
    std::stringstream typeStr;

    switch (type->getTypeID()) {

        case llvm::Type::PointerTyID: {
            auto ptr = (llvm::PointerType *) type;
            std::string elementType = this->llvmTypeToCType(ptr->getElementType());
            // TODO yet another naive instance that I might get away with for now...
            if (elementType == "char") // Make char pointers const-char
                elementType = "const char";
            typeStr << elementType << "*";
            return typeStr.str();
        }

        case llvm::Type::IntegerTyID: {
            auto intType = (llvm::IntegerType *) type;
            if (intType->getBitWidth() == boolWidth) {
                typeStr << "bool";
            } else if (intType->getBitWidth() == charWidth) {
                typeStr << "char";
            } else if (intType->getBitWidth() == shortWidth) {
                typeStr << "short";
            } else if (intType->getBitWidth() == intWidth) {
                typeStr << "int";
            } else if (intType->getBitWidth() == longWidth) {
                typeStr << "long";
            }

            break;
        }

        default:
            throw std::runtime_error("Invalid type");
    }

    return typeStr.str();
}

std::string aotrc::compiler::Compiler::emitHeaderFile(const std::string &module, const std::string &outputPath) {
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
        headerOutput << this->llvmTypeToCType(func.getReturnType()) << ' ' << func.getName().str() << '(';
        // comma separated list of all the parameters
        if (!func.arg_empty()) {
            auto argIt = func.arg_begin();
            for (; argIt != func.arg_end() - 1; ++argIt) {
                headerOutput << this->llvmTypeToCType(argIt->getType());
                if (argIt->hasName()) {
                    headerOutput << argIt->getName().str();
                }
                headerOutput << ", ";
            }
            // Do the last arg
            headerOutput << this->llvmTypeToCType(argIt->getType());
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

std::string aotrc::compiler::Compiler::emitHeaderFile(const std::string &module) {
    std::string defaultName = module + ".h";
    return this->emitHeaderFile(module, defaultName);
}
