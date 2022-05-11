//
// Created by charlie on 5/10/22.
//

#ifndef AOTRC_INSTRUCTION_H
#define AOTRC_INSTRUCTION_H

#include <string>
#include <memory>
#include <variant>
#include "src/fa/transition_edge.h"
#include "util.h"

namespace aotrc::compiler {

    enum InstructionType {
        START_STATE,
        CONSUME,
        CHECK_END,
        DECLARE_VAR,
        TEST,
        GOTO,
        ACCEPT,
        REJECT,
    };

    enum VariableType {
        SIZE,
        CHAR,
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

        DeclareVarInstruction(std::string name, VariableType varType);
        DeclareVarInstruction(std::string name, VariableType varType, std::variant<size_t, char, bool> initialVal);

        std::string str() const noexcept override;

        std::string name;
        VariableType varType;
        std::variant<size_t, char, bool> initialValue;
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

        bool isConditional() const noexcept {
            // The !! needs to coalese the testInstruction into a boolean
            return !!testInstruction;
        }

        unsigned int destId;
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
