//
// Created by charlie on 5/10/22.
//

#include "instruction.h"

#include <sstream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>

std::string aotrc::compiler::StartStateInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "\nSTART_STATE " << this->stateId;
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

aotrc::compiler::DeclareVarInstruction::DeclareVarInstruction(std::string name, llvm::Type *type)
: Instruction(InstructionType::DECLARE_VAR)
, name(std::move(name))
, varType(type) {
    this->initialValue = llvm::ConstantInt::get(this->varType, 0);
}

aotrc::compiler::DeclareVarInstruction::DeclareVarInstruction(std::string name, llvm::LLVMContext &ctx, VariableType varType)
: Instruction(InstructionType::DECLARE_VAR)
, name(std::move(name)) {
    switch (varType) {
        case SIZE:
            this->varType = llvm::IntegerType::getInt64Ty(ctx);
            break;
        case CHAR:
            this->varType = llvm::IntegerType::getInt8Ty(ctx);
            break;
        case BOOL:
            this->varType = llvm::IntegerType::getInt1Ty(ctx);
            break;
    }
    this->initialValue = llvm::ConstantInt::get(this->varType, 0);
}

aotrc::compiler::DeclareVarInstruction::DeclareVarInstruction(std::string name, llvm::Type *type,
                                                              llvm::Value *initialValue)
: Instruction(InstructionType::DECLARE_VAR)
, name(std::move(name))
, varType(type)
, initialValue(initialValue) {
}

std::string aotrc::compiler::DeclareVarInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "DECLARE " << this->name;

    return ss.str();
}

std::string aotrc::compiler::StoreVarInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "STORE " << this->destVar << " <- " << this->sourceVar;

    return ss.str();
}
