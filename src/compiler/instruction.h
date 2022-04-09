//
// Created by charlie on 3/13/22.
//

#ifndef ATORC_INSTRUCTION_H
#define ATORC_INSTRUCTION_H

#include <string>
#include <memory>
#include <utility>
#include <llvm/IR/Value.h>
#include "../fa/transition_edge.h"
#include "program_state.h"

namespace aotrc::compiler {

    enum InstructionType {
        START_STATE,
        ENTER_ACCEPT,
        CONSUME,
        CHECK_END,
        TEST,
        TEST_VAR,
        GOTO,
        ACCEPT,
        REJECT,
    };

    /**
     * A high level regular expression instruction. These are designed to
     * describe the general outline of a regular expression program. This
     */
    class Instruction {
    public:
        /**
         * Gets the instruction type. This is used for instruction inspections
         * @return instruction type
         */
        virtual InstructionType type() const noexcept = 0;

        /**
         * String representation of the current instruction
         * @return
         */
        virtual std::string str() const noexcept = 0;

        /**
         * Emits llvm IR for the given instruction. Each thing takes in a state
         * container which holds all of the state used in the program. This
         * state includes llvm context, variables, and anything potentially needed.
         *
         * NOTE: the return value may be undefined for some instructions
         * @param state Program state to compile
         * @return Optionally some value emitted by IR
         */
        virtual llvm::Value *build(std::unique_ptr<ProgramState> &state) = 0;
    };

    /**
     * This is likely going to be just a NOP instruction. It creates an instruction that
     * shows that a new state is starting. This is really more for readability and debugging.
     */
    class StartStateInstruction : public Instruction {
    public:
        explicit StartStateInstruction(unsigned int id)
        : id(id)
        { }

        InstructionType type() const noexcept override { return InstructionType::START_STATE; }
        std::string str() const noexcept override;

        /**
         * Set the insertion point for the irBuilder at the state specified. Creates a new state if necessary
         * @param state build state
         * @return nullptr always
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;

        /**
         * Get the id of the state being created
         * @return State to start
         */
        unsigned int getId() const { return id; }

    private:
        unsigned int id;
    };

    /**
     * This runs when an accept state is entered. This is useful for when we are sub-matching
     */
    class EnterAcceptInstruction : public Instruction {
    public:
        inline InstructionType type() const noexcept override { return ENTER_ACCEPT; }

        std::string str() const noexcept override;

        /**
         * Stores true into accept state encountered variable. This means that an accept state
         * has been entered, so if we hit a wall, we can accept instead of restart.
         * @param state Program state
         * @return nullptr always
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;
    };

    /**
     * Consumes a character from the subject string for testing. This instruction
     * sets the cursor value.
     */
    class ConsumeInstruction : public Instruction {
    public:
        InstructionType type() const noexcept override { return InstructionType::CONSUME; }
        std::string str() const noexcept override;

        /**
         * Sets the cursor variable and increments the counter variable
         * @param state program state
         * @return nullptr always
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;
    };

    /**
     * Checks if the cursor as at the end of the subject string. If it is,
     * then the on true instruction is executed. Otherwise, the instruction
     * falls through to the next.
     */
    class CheckEndInstruction : public Instruction {
    public:
        explicit CheckEndInstruction(std::unique_ptr<Instruction> on_true_inst)
        : onTrueInst(std::move(on_true_inst))
        {}

        InstructionType type() const noexcept override { return InstructionType::CHECK_END; }
        std::string str() const noexcept override;

        /**
         * Checks if execution has reached the end of the subject string. This instruction has
         * a branch in it, so it inserts a basic block after the current insertion point and
         * sets the insertion point there. This move acts as a fall through (i.e. if the condition
         * is not taken, then there is no "else" condition)
         * @param state compilation state
         * @return      the fall through basic block created by the branch
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;

    private:
        std::unique_ptr<Instruction> onTrueInst;
    };

    /**
     * Tests the current value in the program state against the provided ranges.
     */
    class TestInstruction : public Instruction {
    public:
        explicit TestInstruction(std::vector<fa::Range> ranges)
        : ranges(std::move(ranges))
        {  }

        InstructionType type() const noexcept override { return InstructionType::TEST; }
        std::string str() const noexcept override;

        /**
         * Tests the current cursor value to see if it falls within any of the given ranges.
         * @param state program state
         * @return      True value of if the cursor falls within any ranges
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;

    private:
        std::vector<fa::Range> ranges;
    };

    /**
     * Test's a command and executes the if/else instructions
     */
    class TestVarInstruction : public Instruction {
    public:
        TestVarInstruction(llvm::Value *variable, std::unique_ptr<Instruction> trueCommand, std::unique_ptr<Instruction> falseCommand)
        : variable(variable)
        , trueCommand(std::move(trueCommand))
        , falseCommand(std::move(falseCommand))
        {}

        InstructionType type() const noexcept override { return InstructionType::TEST_VAR; }
        std::string str() const noexcept override;
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;

    private:
        llvm::Value *variable;
        std::unique_ptr<Instruction> trueCommand;
        std::unique_ptr<Instruction> falseCommand;
    };

    /**
     * Jump to a new state, either conditionally or unconditionally. An optional
     * test instruction can be added to test if the goto should be executed.
     */
    class GotoInstruction : public Instruction {
    public:
        GotoInstruction(unsigned int destId)
        : testInst(nullptr)
        , destId(destId)
        {  }

        explicit GotoInstruction(unsigned int destId, std::unique_ptr<TestInstruction> testInstruction)
        : testInst(std::move(testInstruction))
        , destId(destId)
        {  }

        InstructionType type() const noexcept override { return InstructionType::GOTO; }
        std::string str() const noexcept override;

        /**
         * Builds a branch statement. If there is a testInst, then it will try the condition and then
         * jump to the destination. If there is no test instruction, then the jump will be unconditional.
         * @param state Program state
         * @return      If conditional, then the fallthrough block; otherwise nullptr
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;

    private:
        std::unique_ptr<TestInstruction> testInst;
        unsigned int destId;
    };

    /**
     * Accepts the given subject string.
     */
    class AcceptInstruction : public Instruction {
    public:
        InstructionType type() const noexcept override { return InstructionType::ACCEPT; }
        std::string str() const noexcept override;

        /**
         * Just unconditionally branches to the accept block
         * @param state program state
         * @return      nullptr
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;
    };

    /**
     * Rejects the given subject string.
     */
    class RejectInstruction : public Instruction {
    public:
        InstructionType type() const noexcept override { return InstructionType::REJECT; }
        std::string str() const noexcept override;

        /**
         * Just unconditionally branches to the reject block
         * @param state program state
         * @return      nullptr
         */
        llvm::Value *build(std::unique_ptr<ProgramState> &state) override;
    };
}

#endif //ATORC_INSTRUCTION_H
