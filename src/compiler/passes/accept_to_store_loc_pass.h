//
// Created by charlie on 4/9/22.
//

#ifndef AOTRC_ACCEPT_TO_STORE_LOC_PASS_H
#define AOTRC_ACCEPT_TO_STORE_LOC_PASS_H

#include "pass.h"

namespace aotrc::compiler {

    /**
     * A pass that replaces all accept instructions with store location accepts. This is used
     * by the search program.
     */
    class AcceptToStoreLocPass : public Pass {
    public:
        bool accepts(std::unique_ptr<Instruction> &inst) override;

        InstructionPos
        modify(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &instList) override;
    };
}

#endif //AOTRC_ACCEPT_TO_STORE_LOC_PASS_H
