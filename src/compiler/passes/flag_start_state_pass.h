//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_FLAG_START_STATE_PASS_H
#define AOTRC_FLAG_START_STATE_PASS_H

#include "pass.h"
#include "src/fa/dfa.h"

namespace aotrc::compiler {

    /**
     * This pass replaces the start state with the enter start state instruction for all accept
     * states. The instructions set within the state that an accept state has been encountered.
     * This is useful for substate matching
     */
    class FlagStartStatePass : public Pass {
    public:
        /**
         * Creates a new pass. The DFA is provided so to tell which states are accept states.
         * TODO a better solution could be to pass a set of accept states instead
         * @param dfa DFA that is being constructed
         */
        explicit FlagStartStatePass(const aotrc::fa::DFA &dfa)
        : dfa(dfa)
        { }

        bool accepts(std::unique_ptr<Instruction> &inst) override;

        InstructionPos modify(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &instList) override;

    private:
        const aotrc::fa::DFA &dfa;
    };
}

#endif //AOTRC_FLAG_START_STATE_PASS_H
