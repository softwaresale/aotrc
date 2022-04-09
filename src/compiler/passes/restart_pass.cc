//
// Created by charlie on 3/18/22.
//

#include "restart_pass.h"

bool aotrc::compiler::RestartPass::accepts(std::unique_ptr<Instruction> &inst) {
    return inst->type() == InstructionType::REJECT;
}

aotrc::compiler::Pass::InstructionPos
aotrc::compiler::RestartPass::modify(std::unique_ptr<Instruction> &inst, aotrc::compiler::Pass::InstructionPos pos, InstructionList &list) {
    // Create a new GOTO state 0 instruction
    // TODO using a magic number here. Swap this for some sort of constant
    auto gotoInst = std::make_unique<GotoInstruction>(0);
    auto gotoInstCast = std::unique_ptr<Instruction>(std::move(gotoInst));
    // Make it so that inst now holds to GOTO, and now gotoInstCast holds the reject
    inst.swap(gotoInstCast);
    // The gotoInstCast gets dropped
    return pos;
}
