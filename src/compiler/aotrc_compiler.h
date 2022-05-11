//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_AOTRC_COMPILER_H
#define AOTRC_AOTRC_COMPILER_H

#include <string>
#include <optional>
#include <llvm/IR/Type.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include "src/fa/dfa.h"

namespace aotrc::compiler {
    /**
     * The *big* compiler. Facade around all of the regex compiler types. Does other stuff too.
     */
    class AotrcCompiler {
    public:
        AotrcCompiler();

        /**
        * Builds a full match regex.
        * @param module Name of module to place the regex in
        * @param label Name of the regex
        * @param regex The pattern for the regex
        * @param genPatternFunc If a get pattern function should be compiled for the regex
        * @return True if successful.
        */
        void compileRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA);

        void compileSubMatchRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA);

        std::string emitIr(const std::string &module);

        std::string emitAssembly(const std::string &module);

        std::string emitAssembly(const std::string &module, const std::string &outputPath);

        std::string emitObjectFile(const std::string &module);

        std::string emitObjectFile(const std::string &module, const std::string &outputPath);

        std::string emitHeaderFile(const std::string &module);

        std::string emitHeaderFile(const std::string &module, const std::string &outputPath);

        void generatePatternFunc(const std::string &module, const std::string &label, const std::string &regex);

    private:

        std::string llvmTypeToCType(llvm::Type *type);

        std::string emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type);

        llvm::LLVMContext llvmContext;
        std::unordered_map <std::string, std::unique_ptr<llvm::Module>> modules;
        const llvm::Target *target;
        std::unique_ptr <llvm::TargetMachine> targetMachine;
    };
};


#endif //AOTRC_AOTRC_COMPILER_H
