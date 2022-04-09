//
// Created by charlie on 3/26/22.
//

#include "flag_start_state_pass.h"

bool aotrc::compiler::FlagStartStatePass::accepts(std::unique_ptr<Instruction> &inst) {
    return inst->type() == InstructionType::START_STATE;
}

aotrc::compiler::Pass::InstructionPos aotrc::compiler::FlagStartStatePass::modify(std::unique_ptr<Instruction> &inst,
                                                                                  aotrc::compiler::Pass::InstructionPos pos,
                                                                                  InstructionList &instList) {
    // Convert to a start state instruction
    auto startStateInst = static_cast<StartStateInstruction *>(inst.get()); // this is checked...
    if (this->dfa.isAcceptState(startStateInst->getId())) {
        // If this is a start state, then create an enter accept instruction
        auto enterInst = std::make_unique<aotrc::compiler::EnterAcceptInstruction>();
        auto insertBefore = std::next(pos);
        return instList.insert(insertBefore, std::move(enterInst));
    } else {
        return pos;
    }
}
