//
// Created by charlie on 3/13/22.
//

#ifndef ATORC_PROGRAM_H
#define ATORC_PROGRAM_H

#include "instruction.h"
#include "../fa/dfa.h"
#include "src/compiler/passes/pass.h"
#include "program_mode.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace aotrc::compiler {

    /**
     * A high level regular expression program. These program is designed to transform a
     * finite automaton into a "program," which is a sequence of instructions. This program
     * can be modified and optimized, and then converted into real machine code.
     *
     * TODO: 1) add a mode parameter that gives the mode of the program (should inform the name and parameters)
     */
    class Program {
    public:
        // Used for the position
        using InstructionPos = std::vector<std::unique_ptr<aotrc::compiler::Instruction>>::iterator;

        /**
         * Compile a new regular expression program from a dfa
         * @param dfa regex to compile
         */
        Program(std::string name, ProgramMode progType, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &module);

        /**
         * Builds the program for a given dfa
         * @param dfa dfa to build
         */
        virtual void build(const fa::DFA &dfa);

        /**
         * Run a pass on this program. This pass may modify the instructions of this
         * program.
         * @param pass Pass to run
         */
        void runPass(std::unique_ptr<Pass> &pass);

        /**
         * Compiles the current program
         */
        virtual void compile();

        const std::vector<std::unique_ptr<Instruction>> &getInstructions() const {
            return this->instructions;
        }

    protected:
        std::string name;
        llvm::Function *function;
        /// Vector of instructions to be read simultaneously
        std::vector<std::unique_ptr<Instruction>> instructions;
        std::unique_ptr<ProgramState> programState;
    };

    std::ostream &operator<<(std::ostream &os, const Program &program);

    /**
     * A very simple implementation of program. This class represents the default mode of the program, which is
     * full matching. No additional modifications must be made.
     */
    class FullMatchProgram : public Program {
    public:
        FullMatchProgram(std::string name, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &module)
        : Program(std::move(name), ProgramMode::FULL_MATCH, ctx, module)
        {}
    };
}

#endif //ATORC_PROGRAM_H
