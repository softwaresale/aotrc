//
// Created by charlie on 3/18/22.
//

#include "pass.h"

aotrc::compiler::Pass::InstructionPos
aotrc::compiler::Pass::operator()(std::unique_ptr<Instruction> &inst, aotrc::compiler::Pass::InstructionPos pos, InstructionList &instList) {
    if (this->operation) {
        OperatorType lambdaOperation = *this->operation;
        return std::invoke(lambdaOperation, inst, pos, instList);
    } else {
        if (this->accepts(inst)) {
            return this->modify(inst, pos, instList);
        } else {
            return pos;
        }
    }
}
