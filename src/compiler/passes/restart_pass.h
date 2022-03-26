//
// Created by charlie on 3/18/22.
//

#ifndef AOTRC_RESTART_PASS_H
#define AOTRC_RESTART_PASS_H

#include "pass.h"

namespace aotrc::compiler {

    /**
     * This pass goes through and replaces all of the default rejects with instructions
     * to go back to state one instead of rejecting.
     */
    class RestartPass : public Pass {
    public:
        RestartPass()
        : Pass() { }

        /**
         * Modify rejects statements
         * @param inst Instruction to modify
         * @return True if the instruction is a reject instruction
         */
        bool accepts(std::unique_ptr<Instruction> &inst) override;

        /**
         * Swaps with a GOTO state 0
         * @param inst Reject instruction
         */
        InstructionPos modify(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &list) override;
    };
}

#endif //AOTRC_RESTART_PASS_H
