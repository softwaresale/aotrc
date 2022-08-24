//
// Created by charlie on 5/10/22.
//

#ifndef AOTRC_INSTRUCTION_H
#define AOTRC_INSTRUCTION_H

#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include "src/fa/transition_edge.h"
#include "util.h"

namespace aotrc::compiler {

    /**
     * Describes the different type of instructions
     */
    enum InstructionType {
        /// Starts a new DFA state
        START_STATE,
        /// Take the next input character from the subject string
        CONSUME,
        /// Check if the end of the subject string is reached
        CHECK_END,
        /// Create a new variable
        DECLARE_VAR,
        /// Store a value into a variable
        STORE_VAR,
        /// See if a character fits within an edge
        TEST,
        /// Go to another state, potentially conditionally
        GOTO,
        /// Accept the subject string
        ACCEPT,
        /// Reject the subject string
        REJECT,
    };

    /**
     * Describes the types of variables that can be declared. There
     * are only a limited number as the programs do not need lots of different
     * kinds of variables
     */
    enum VariableType {
        /// An unsigned 64-bit integer
        SIZE,
        /// An 8-bit character
        CHAR,
        /// A boolean value
        BOOL,
    };

    /**
     * A basic high-level instruction. These instructions help to describe the DFA and
     * represent it as a linear set of instructions
     */
    struct Instruction {
        explicit Instruction(InstructionType type)
        : instType(type)
        {}

        virtual ~Instruction() = default;

        /**
         * Get the type of the instruction
         * @return Type of instruction
         */
        [[nodiscard]] InstructionType type() const noexcept {
            return this->instType;
        };

        /**
         * Get the stringified representation of the instruction
         * @return String representation of the instruction
         */
        [[nodiscard]] virtual std::string str() const noexcept = 0;

    protected:
        InstructionType instType;
    };

    /**
     * Convenience wrapper over unique_ptr of instruction
     */
    using InstructionPtr = std::unique_ptr<Instruction>;

    /**
     * Declares a new variable
     */
    struct DeclareVarInstruction : public Instruction {

        DeclareVarInstruction(std::string name, llvm::Type *varType);
        DeclareVarInstruction(std::string name, llvm::LLVMContext &ctx, VariableType varType);
        DeclareVarInstruction(std::string name, llvm::Type *varType, llvm::Value *initialValue);

        std::string str() const noexcept override;

        /// The name of the variable
        std::string name;
        /// The type of the variable
        llvm::Type *varType;
        /// Some sort of initial value the variable should hold
        llvm::Value *initialValue;
    };

    /**
     * Stores a value into a variable from another variable's value
     */
    struct StoreVarInstruction : public Instruction {
        StoreVarInstruction(std::string destName, std::string sourceName)
        : Instruction(InstructionType::STORE_VAR)
        , destVar(std::move(destName))
        , sourceVar(std::move(sourceName))
        {}

        std::string str() const noexcept override;

        std::string destVar;
        std::string sourceVar;
    };

    /**
     * Used to define that we have started a new state.
     */
    struct StartStateInstruction : public Instruction {
        StartStateInstruction(unsigned int state, bool isAccept)
        : Instruction(InstructionType::START_STATE)
        , stateId(state)
        , isAccept(isAccept)
        {}

        std::string str() const noexcept override;

        /**
         * The label of this state
         */
        std::string getStateLabel() const noexcept {
            return aotrc::compiler::getStateBlockLabel(this->stateId);
        }

        unsigned int stateId;
        bool isAccept;
    };

    /**
     * Consumes a new character from the subject string and operates on it
     */
    struct ConsumeInstruction : public Instruction {
        ConsumeInstruction()
        : Instruction(InstructionType::CONSUME)
        {}

        std::string str() const noexcept override;
    };

    /**
     * Checks if we are at the end of the subject string
     */
    struct CheckEndInstruction : public Instruction {
        explicit CheckEndInstruction(std::unique_ptr<Instruction> onTrue)
        : Instruction(InstructionType::CHECK_END)
        , onTrueInst(std::move(onTrue))
        {}

        std::string str() const noexcept override;

        /// Instruction to be executed if we are at the end
        std::unique_ptr<Instruction> onTrueInst;
    };

    /**
     * Tests if an edge can be taken on the graph
     */
    struct TestEdgeInstruction : public Instruction {
        explicit TestEdgeInstruction(const aotrc::fa::Edge &edgeToTest)
        : Instruction(InstructionType::TEST)
        , edgeToTest(edgeToTest)
        {}

        std::string str() const noexcept override;

        /// The edge to test if any of the ranges fit.
        aotrc::fa::Edge edgeToTest;
    };

    /**
     * Transitions to another state, either conditionally or unconditionally
     */
    struct GoToInstruction : public Instruction {

        explicit GoToInstruction(unsigned int destId)
        : Instruction(InstructionType::GOTO)
        , destId(destId)
        , testInstruction(nullptr)
        {}

        GoToInstruction(unsigned int destId, std::unique_ptr<Instruction> testInst)
        : Instruction(InstructionType::GOTO)
        , destId(destId)
        , testInstruction(std::move(testInst))
        {}

        std::string str() const noexcept override;

        /**
         * True if the instruction has a condition for if it can jump
         * @return True if conditional
         */
        bool isConditional() const noexcept {
            // The !! needs to coalese the testInstruction into a boolean
            return !!testInstruction;
        }

        /// The state to transition to
        unsigned int destId;
        /// Optional instruction to see if we can jump
        std::unique_ptr<Instruction> testInstruction;
    };

    /**
     * Accepts, terminating execution
     */
    struct AcceptInstruction : public Instruction {
        AcceptInstruction()
        : Instruction(InstructionType::ACCEPT)
        {}

        std::string str() const noexcept override;
    };

    /**
     * Rejects, terminating execution
     */
    struct RejectInstruction : public Instruction {
        RejectInstruction()
        : Instruction(InstructionType::REJECT)
        {}

        std::string str() const noexcept override;
    };
}

#endif //AOTRC_INSTRUCTION_H
