//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_REJECT_TO_CONDITIONAL_H
#define AOTRC_REJECT_TO_CONDITIONAL_H

#include "pass.h"
#include "src/fa/dfa.h"

namespace aotrc::compiler {
    class RejectToConditionalPass : public Pass {
    public:
        explicit RejectToConditionalPass(const aotrc::fa::DFA &dfa)
        : dfa(dfa)
        { }

        bool accepts(std::unique_ptr<Instruction> &inst) override;

        InstructionPos
        modify(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &instList) override;

    private:
        const aotrc::fa::DFA &dfa;
    };
}

#endif //AOTRC_REJECT_TO_CONDITIONAL_H
