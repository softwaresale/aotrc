//
// Created by charlie on 5/10/22.
//

#include "instruction.h"

#include <sstream>

std::string aotrc::compiler::StartStateInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "START_STATE " << this->stateId;
    if (this->isAccept) {
        ss << " **";
    }

    return ss.str();
}

std::string aotrc::compiler::ConsumeInstruction::str() const noexcept {
    return "CONSUME";
}

std::string aotrc::compiler::CheckEndInstruction::str() const noexcept {
    return "CHECK_END:\n\t" + this->onTrueInst->str();
}

std::string aotrc::compiler::TestEdgeInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "TEST_EDGE " << this->edgeToTest;
    return ss.str();
}

std::string aotrc::compiler::GoToInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "GOTO ";
    if (this->testInstruction) {
        ss << "{" << testInstruction->str() << "} ";
    }
    ss << this->destId;

    return ss.str();
}

std::string aotrc::compiler::AcceptInstruction::str() const noexcept {
    return "ACCEPT";
}

std::string aotrc::compiler::RejectInstruction::str() const noexcept {
    return "REJECT";
}

aotrc::compiler::DeclareVarInstruction::DeclareVarInstruction(std::string name, aotrc::compiler::VariableType varType)
: Instruction(InstructionType::DECLARE_VAR)
, name(std::move(name))
, varType(varType) {

    if (varType == VariableType::BOOL) {
        this->initialValue = false;
    } else if (varType == VariableType::SIZE) {
        this->initialValue.emplace<size_t>(0);
    } else {
        this->initialValue.emplace<char>(0);
    }

}

aotrc::compiler::DeclareVarInstruction::DeclareVarInstruction(std::string name, aotrc::compiler::VariableType varType,
                                                              std::variant<size_t, char, bool> initialVal)
: Instruction(InstructionType::DECLARE_VAR)
, name(std::move(name))
, varType(varType)
, initialValue(initialVal) {
    // TODO make this look prettier
    if (initialVal.index() == 0 && varType == VariableType::SIZE) {}
    else if (initialVal.index() == 1 && varType == VariableType::CHAR) {}
    else if (initialVal.index() == 2 && varType == VariableType::BOOL) {}
    else {
        throw std::runtime_error("DeclareVarInstruction - incompatible type and initial value");
    }
}

static std::ostream &print_literal(std::ostream &os, const std::variant<size_t, char, bool> &val) {
    if (val.index() == 0) {
        os << std::get<size_t>(val);
    } else if (val.index() == 1) {
        os << std::get<char>(val);
    } else {
        os << std::boolalpha << std::get<bool>(val);
    }

    return os;
}

std::string aotrc::compiler::DeclareVarInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "DECLARE " << this->name << ": " << this->varType << " = ";
    print_literal(ss, this->initialValue);

    return ss.str();
}

std::string aotrc::compiler::StoreVarInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "STORE " << this->destVar << " <- " << this->sourceVar;

    return ss.str();
}
