//
// Created by charlie on 3/13/22.
//

#ifndef ATORC_COMPILER_H
#define ATORC_COMPILER_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

namespace aotrc::compiler {
    /**
     * The entire compiler. This is what takes in a DFA and generates machine executable code
     */
    class Compiler {
    public:
        Compiler();

        bool compileRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc = true);

        void emitIr(const std::string &module);
        void emitAssembly(const std::string &module);
        void emitAssembly(const std::string &module, const std::string &outputPath);
        void emitObjectFile(const std::string &module);
        void emitObjectFile(const std::string &module, const std::string &outputPath);
        void emitHeaderFile(const std::string &module);
        void emitHeaderFile(const std::string &module, const std::string &outputPath);

    private:
        bool generatePatternFunc(const std::string &module, const std::string &label, const std::string &regex);
        std::string llvmTypeToCType(llvm::Type *type);
        void emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type);

        llvm::LLVMContext llvmContext;
        std::unordered_map<std::string, std::unique_ptr<llvm::Module>> modules;
        const llvm::Target *target;
        std::unique_ptr<llvm::TargetMachine> targetMachine;
    };
}

#endif //ATORC_COMPILER_H
