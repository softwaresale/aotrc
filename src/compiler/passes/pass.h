//
// Created by charlie on 3/18/22.
//

#ifndef AOTRC_PASS_H
#define AOTRC_PASS_H

#include "src/compiler/instruction.h"
#include <functional>
#include <memory>
#include <optional>

namespace aotrc::compiler {
    /**
     * A pass is used for passing over the instructions in a program and modifying them. A combination
     * of passes can be used to transform a program.
     */
    class Pass {
    public:

        /**
         * Convenience for vector of instruction pointers
         */
        using InstructionList = std::vector<std::unique_ptr<Instruction>>;

        /**
         * Represents the position of an instruction in the instruction vector
         */
        using InstructionPos = std::vector<std::unique_ptr<Instruction>>::iterator;

        /**
         * The type of a pass operation.
         */
        using OperatorType = std::function<InstructionPos(std::unique_ptr<Instruction>&, InstructionPos, InstructionList&)>;

        /**
         * Initialize a pass without an operation. When this is the case, then the pass class
         * is expected to be overloaded instead.
         */
        Pass()
        : operation()
        { }

        /**
         * Create a new pass out of a task. If a task is passed, then the accepts and modifies functions
         * need not be implemented as a passed operation takes precedence over local operation.
         * @param operation The operation to run on each instruction
         */
        explicit Pass(OperatorType operation)
        : operation(std::move(operation))
        { }

        /**
         * Calls the operation on an instruction. This method will determine if the overloaded operation mechanism
         * should be used or if a provided operation should be used.
         * @param inst Instruction to operate on
         * @param pos  Position of the instruction being modified
         * @param instList List of instructions that is being passed over
         */
        InstructionPos operator()(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &instList);

        /**
         * Internal operation method. This function is used to determine if this pass should modify
         * the given instruction.
         * @param inst Instruction to potentially modify
         * @return True if it should modify, false otherwise
         */
        virtual bool accepts(std::unique_ptr<Instruction> &inst) {
            throw std::runtime_error("Accepts not implemented");
        }

        /**
         * Modifies an existing instruction. `accept` is used to determine if the instruction should
         * be modified
         * @param inst Instruction to modify
         */
        virtual InstructionPos modify(std::unique_ptr<Instruction> &inst, InstructionPos pos, InstructionList &instList) {
            throw std::runtime_error("Modify not implemented");
        }

    private:
        std::optional<OperatorType> operation;
    };
}

#endif //AOTRC_PASS_H
