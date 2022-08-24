//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_DFA_COMPILER_H
#define AOTRC_DFA_COMPILER_H

#include <vector>
#include "instruction.h"
#include "src/fa/dfa.h"

namespace aotrc::compiler {
    /**
     * Used to compile a DFA into a list of high-level instructions. Responsible for turning a DFA into
     * a sequence of HIR instructions.
     */
    class DFACompiler {
    public:
        explicit DFACompiler(llvm::LLVMContext &ctx)
        : ctx(ctx)
        {}

        virtual ~DFACompiler() = default;

        /**
         * Builds setup of the program, which in includes any instructions that should occur
         * before evaluating the actual DFA. This can be optional.
         * @return A sequence of instructions to be run before
         */
        virtual std::vector<InstructionPtr> buildSetup() = 0;

        /**
         * Builds out the instructions used in a DFA state. This pattern assumes that each state should be
         * built out identically. The emits the sequence of instructions used for a given state.
         * @param state Id of state to build out
         * @param dfa The DFA that the state belongs to
         * @return A sequence of instructions that represent the given state
         */
        virtual std::vector<InstructionPtr> buildState(unsigned int state, const aotrc::fa::DFA &dfa) = 0;

    protected:
        llvm::LLVMContext &ctx;
    };
}

#endif //AOTRC_DFA_COMPILER_H
