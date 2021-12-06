//
// Created by charlie on 12/6/21.
//

#include "code_gen.h"

#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/IR/LegacyPassManager.h>

aotrc::compiler::CodeGen::CodeGen() {
    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(LLVM_DEFAULT_TARGET_TRIPLE, error);
    if (!target) {
        throw std::runtime_error(error);
    }

    llvm::TargetOptions options;
    this->machine = std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(LLVM_DEFAULT_TARGET_TRIPLE, "generic", "", options, llvm::Optional<llvm::Reloc::Model>()));
}

bool aotrc::compiler::CodeGen::compileModule(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath, llvm::CodeGenFileType outputType) {
    // Set up some basics of the module
    module->setDataLayout(this->machine->createDataLayout());
    module->setCodeModel(this->machine->getCodeModel());

    // Open the file to emit to
    std::error_code err;
    llvm::raw_fd_ostream outputStream(outputPath, err);
    if (err) {
        llvm::errs() << "Could not open file: " << err.message();
        return false;
    }

    // Add a pass to generate machine code
    llvm::legacy::PassManager passManager;
    if (this->machine->addPassesToEmitFile(passManager, outputStream, nullptr, outputType)) {
        llvm::errs() << "Error while emitting to file";
        return false;
    }

    // Run all the passes
    passManager.run(*module);

    // Flush the stream and such
    outputStream.flush();

    return true;
}
