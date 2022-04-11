//
// Created by charlie on 4/9/22.
//

#include "accept_to_store_loc_pass.h"

bool aotrc::compiler::AcceptToStoreLocPass::accepts(std::unique_ptr<Instruction> &inst) {
    if (inst->type() == ACCEPT)
        return true;
    else if (inst->type() == CHECK_END) {
        auto checkEnd = static_cast<CheckEndInstruction*>(inst.get());
        if (checkEnd->getOnTrueInst()->type() == ACCEPT) {
            return true;
        }
    }

    return false;
}

aotrc::compiler::Pass::InstructionPos aotrc::compiler::AcceptToStoreLocPass::modify(std::unique_ptr<Instruction> &inst,
                                                                                    aotrc::compiler::Pass::InstructionPos pos,
                                                                                    aotrc::compiler::Pass::InstructionList &instList) {
    // Make a new store loc accept instruction
    std::unique_ptr<Instruction> replacement = std::make_unique<StoreLocationAcceptInstruction>();
    if (inst->type() == CHECK_END) {
        replacement = std::make_unique<CheckEndInstruction>(std::move(replacement));
    }

    inst.swap(replacement);

    return pos;
}
