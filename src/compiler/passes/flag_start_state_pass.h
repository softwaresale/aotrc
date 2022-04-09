//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_FLAG_START_STATE_PASS_H
#define AOTRC_FLAG_START_STATE_PASS_H

#include "pass.h"
#include "src/fa/dfa.h"

namespace aotrc::compiler {
    class FlagStartStatePass : public Pass {
    public:
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
