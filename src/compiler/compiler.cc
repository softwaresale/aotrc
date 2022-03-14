//
// Created by charlie on 3/13/22.
//

#include "compiler.h"

#include <iostream>
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
    auto nfa = aotrc::parser::parse_regex(regex);
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
