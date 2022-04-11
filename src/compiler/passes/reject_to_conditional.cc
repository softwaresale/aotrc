//
// Created by charlie on 3/26/22.
//

#include "reject_to_conditional.h"

bool aotrc::compiler::RejectToConditionalPass::accepts(std::unique_ptr<Instruction> &inst) {
    return inst->type() == REJECT;
}

aotrc::compiler::Pass::InstructionPos
aotrc::compiler::RejectToConditionalPass::modify(std::unique_ptr<Instruction> &inst,
                                                 aotrc::compiler::Pass::InstructionPos pos,
                                                 aotrc::compiler::Pass::InstructionList &instList) {

    // Find the most recent start state
    bool found = false;
    unsigned int destState = 0;
    InstructionList::reverse_iterator backwards_pos(pos);
    for (auto it = backwards_pos; it != instList.rend(); ++it) {
        if ((*it)->type() == START_STATE) {
            destState = dynamic_cast<StartStateInstruction*>(it->get())->getId();
            found = true;
            break;
        }
    }

    if (!found) {
        throw std::runtime_error("Could not find a START_STATE instruction");
    }

    // If dest state is an accept state, then reject. Otherwise, goto 0
    std::unique_ptr<Instruction> replacement;
    if (this->dfa.isAcceptState(destState)) {
        replacement = std::invoke(this->acceptProducer);
    } else {
        replacement = std::make_unique<GotoInstruction>(dfa.getStartState());
    }

    // Replace inst with replacement instruction
    inst.swap(replacement);

    // We didn't insert anything, so just pass the position through
    return pos;
}
