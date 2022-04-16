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

        /**
        * Builds a full match regex.
        * @param module Name of module to place the regex in
        * @param label Name of the regex
        * @param regex The pattern for the regex
        * @param genPatternFunc If a get pattern function should be compiled for the regex
        * @return True if successful.
        */
        bool compileRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc = true);

        /**
        * Builds a sub match regex program
        * @param module Name of module to place the regex in
        * @param label Name of the regex
        * @param regex The pattern for the regex
        * @param genPatternFunc If a get pattern function should be compiled for the regex
        * @return True if successful.
        */
        bool compileSubmatchRegex(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc = true);

        std::string emitIr(const std::string &module);
        std::string emitAssembly(const std::string &module);
        std::string emitAssembly(const std::string &module, const std::string &outputPath);
        std::string emitObjectFile(const std::string &module);
        std::string emitObjectFile(const std::string &module, const std::string &outputPath);
        std::string emitHeaderFile(const std::string &module);
        std::string emitHeaderFile(const std::string &module, const std::string &outputPath);

    private:
        /**
         * Generalized function for compiling regex programs. This function compiles a program of the given program
         * type.
         * @tparam ProgramTp Type of program to compile. Must be a subclass of Program
         * @tparam ProgramModeTp Type of the program mode that should be associated with the
         * @param module The module to compile into. If the module does not exist, create one
         * @param label Label/name of the regex
         * @param regex The actual regex pattern
         * @param genPatternFunc If a pattern retrieval function should be generated
         * @return True if successful
         */
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

    /**
     * Generalized compilation function. It compiles a type of program with the given regex info and places the
     * result into the provided module.
     * @tparam ProgramTp Type of program to compile. Must be a base class of Program
     * @param module Name of module to place the regex in
     * @param label Name of the regex
     * @param regex The pattern for the regex
     * @param genPatternFunc If a get pattern function should be compiled for the regex
     * @return True if successful.
     */
    template <class ProgramTp>
    bool Compiler::compileProgram(const std::string &module, const std::string &label, const std::string &regex, bool genPatternFunc) {
        // Do some checks
        static_assert(
                std::is_base_of_v<aotrc::compiler::Program<typename ProgramTp::ProgramModeType>, ProgramTp> &&
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
