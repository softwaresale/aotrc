//
// Created by charlie on 3/13/22.
//

#ifndef ATORC_PROGRAM_H
#define ATORC_PROGRAM_H

#include "instruction.h"
#include "../fa/dfa.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace aotrc::compiler {

    /**
     * A high level regular expression program. These program is designed to transform a
     * finite automaton into a "program," which is a sequence of instructions. This program
     * can be modified and optimized, and then converted into real machine code.
     */
    class Program {
    public:
        /**
         * Compile a new regular expression program from a dfa
         * @param dfa regex to compile
         */
        explicit Program(std::string name, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &module);

        /**
         * Builds the program for a given dfa
         * @param dfa dfa to build
         */
        void build(const fa::DFA &dfa);

        /**
         * Compiles the current program
         */
        void compile();

        const std::vector<std::unique_ptr<Instruction>> &getInstructions() const {
            return this->instructions;
        }

    private:
        std::string name;
        llvm::Function *function;
        /// Vector of instructions to be read simultaneously
        std::vector<std::unique_ptr<Instruction>> instructions;
        std::unique_ptr<ProgramState> programState;
    };

    std::ostream &operator<<(std::ostream &os, const Program &program);
}

#endif //ATORC_PROGRAM_H
