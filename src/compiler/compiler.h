//
// Created by charlie on 3/13/22.
//

#ifndef ATORC_COMPILER_H
#define ATORC_COMPILER_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include "src/parser/regex_parser.h"
#include "src/fa/dfa.h"
#include "program_mode.h"
#include "program.h"

namespace aotrc::compiler {
    /**
     * The entire compiler. This is what takes in a DFA and generates machine executable code
     */
    class Compiler {
    public:
        Compiler();

        bool compileRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc = true);
        bool compileSubmatchRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc = true);

        std::string emitIr(const std::string &module);
        std::string emitAssembly(const std::string &module);
        std::string emitAssembly(const std::string &module, const std::string &outputPath);
        std::string emitObjectFile(const std::string &module);
        std::string emitObjectFile(const std::string &module, const std::string &outputPath);
        std::string emitHeaderFile(const std::string &module);
        std::string emitHeaderFile(const std::string &module, const std::string &outputPath);

    private:
        template <class ProgramTp>
        bool compileProgram(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc = true);

        bool generatePatternFunc(const std::string &module, const std::string &label, const std::string &regex);
        std::string llvmTypeToCType(llvm::Type *type);
        std::string emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type);

        llvm::LLVMContext llvmContext;
        std::unordered_map<std::string, std::unique_ptr<llvm::Module>> modules;
        const llvm::Target *target;
        std::unique_ptr<llvm::TargetMachine> targetMachine;
    };

    template <class ProgramTp>
    bool Compiler::compileProgram(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc) {
        // Do some checks
        static_assert(
                std::is_base_of_v<aotrc::compiler::Program, ProgramTp> &&
                std::is_constructible_v<ProgramTp, std::string, llvm::LLVMContext&, const std::unique_ptr<llvm::Module> &>
        );

        // Create a new module if necessary
        if (this->modules.find(module) == this->modules.end()) {
            this->modules[module] = std::make_unique<llvm::Module>(module, this->llvmContext);
        }

        // Transform regex from NFA -> DFA -> Program
        aotrc::fa::NFA nfa;
        try {
            nfa = aotrc::parser::parseRegex(regex);
        } catch (std::runtime_error &exe) {
            std::stringstream msg;
            msg << "Error while parsing regex /" << regex << "/: " << exe.what();
            throw std::runtime_error(msg.str());
        }
        aotrc::fa::DFA dfa(nfa);
        ProgramTp program(label, this->llvmContext, this->modules[module]);

        // build the program
        program.build(dfa);

        // Compile the program
        program.compile();

        if (genPatternFunc) {
            return this->generatePatternFunc(module, label, regex);
        }

        // Done
        return true;
    }
}

#endif //ATORC_COMPILER_H
