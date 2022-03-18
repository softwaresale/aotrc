//
// Created by charlie on 3/13/22.
//

#include "compiler.h"

#include <iostream>
#include <fstream>
#include "../parser/regex_parser.h"
#include "../fa/dfa.h"
#include "program.h"

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

bool aotrc::compiler::Compiler::compileRegex(const std::string &module, const std::string &label, const std::string &regex) {
    // Create a new module if necessary
    if (this->modules.find(module) == this->modules.end()) {
        this->modules[module] = std::make_unique<llvm::Module>(module, this->llvmContext);
    }

    // Transform regex from NFA -> DFA -> Program
    auto nfa = aotrc::parser::parseRegex(regex);
    fa::DFA dfa(nfa);
    Program program(label, this->llvmContext, this->modules[module]);

    // build the program
    program.build(dfa);

    // Compile the program
    program.compile();

    // Done
    return true;
}

void aotrc::compiler::Compiler::emitIr(const std::string &module) {
    auto &mod = this->modules.at(module);
    std::string output;
    llvm::raw_string_ostream os(output);
    os << *mod;
    std::cout << output << std::endl;
}

void aotrc::compiler::Compiler::emitAssembly(const std::string &module, const std::string &outputPath) {
    this->emitCode(module, outputPath, llvm::CGFT_AssemblyFile);
}

void aotrc::compiler::Compiler::emitAssembly(const std::string &module) {
    std::string defaultName = module + ".s";
    this->emitAssembly(module, defaultName);
}

void aotrc::compiler::Compiler::emitObjectFile(const std::string &module, const std::string &outputPath) {
    this->emitCode(module, outputPath, llvm::CGFT_ObjectFile);
}


void aotrc::compiler::Compiler::emitObjectFile(const std::string &module) {
    std::string defaultName = module + ".o";
    this->emitObjectFile(module, defaultName);
}


void aotrc::compiler::Compiler::emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type) {
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

void aotrc::compiler::Compiler::emitHeaderFile(const std::string &module, const std::string &outputPath) {
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
}

void aotrc::compiler::Compiler::emitHeaderFile(const std::string &module) {
    std::string defaultName = module + ".h";
    this->emitHeaderFile(module, defaultName);
};
