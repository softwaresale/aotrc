//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_REJECT_TO_CONDITIONAL_H
#define AOTRC_REJECT_TO_CONDITIONAL_H

#include "pass.h"
#include "src/fa/dfa.h"

namespace aotrc::compiler {
    /**
     * Pass used in sub matching. All reject instructions at the end of states are replaced with
     * a conditional that checks if an accept state has been encountered. If it has been, then it
     * accepts. Otherwise, it goes back to state 0.
     */
    class RejectToConditionalPass : public Pass {
    public:
        explicit RejectToConditionalPass(const aotrc::fa::DFA &dfa, std::function<std::unique_ptr<Instruction>()> acceptProducer)
        : dfa(dfa)
        , acceptProducer(std::move(acceptProducer))
        { }

        bool accepts(std::unique_ptr<Instruction> &inst) override;

        InstructionPos
        modify(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &instList) override;

    private:
        const aotrc::fa::DFA &dfa;
        std::function<std::unique_ptr<Instruction>()> acceptProducer;
    };
}

#endif //AOTRC_REJECT_TO_CONDITIONAL_H
