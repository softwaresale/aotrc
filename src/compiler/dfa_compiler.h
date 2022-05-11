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
     * Used to compile a DFA into a list of high-level instructions.
     */
    class DFACompiler {
    public:
        /**
         * Builds setup of the program, which in includes any instructions that should occur
         * before evaluating the actual DFA
         * @return A sequence of instructions to be run before
         */
        virtual std::vector<InstructionPtr> buildSetup() = 0;

        /**
         * Builds out the instructions used in a DFA state
         * @param state Id of state to build out
         * @param dfa The DFA that the state belongs to
         * @return A sequence of instructions that represent the given state
         */
        virtual std::vector<InstructionPtr> buildState(unsigned int state, const aotrc::fa::DFA &dfa) = 0;
    };
}

#endif //AOTRC_DFA_COMPILER_H
