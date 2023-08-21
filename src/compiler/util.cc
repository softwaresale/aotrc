//
// Created by charlie on 5/11/22.
//

#include "util.h"
#include <llvm/IR/Constants.h>
#include <sstream>

std::string aotrc::compiler::getStateBlockLabel(unsigned int stateId) {
    return "STATE_" + std::to_string(stateId);
}

std::string aotrc::compiler::llvmTypeToCType(llvm::LLVMContext &ctx, llvm::Type *type) {
    static auto boolWidth = llvm::IntegerType::getInt1Ty(ctx)->getBitWidth();
    static auto charWidth = llvm::IntegerType::getInt8Ty(ctx)->getBitWidth();
    static auto shortWidth = llvm::IntegerType::getInt16Ty(ctx)->getBitWidth();
    static auto intWidth = llvm::IntegerType::getInt32Ty(ctx)->getBitWidth();
    static auto longWidth = llvm::IntegerType::getInt64Ty(ctx)->getBitWidth();
    std::stringstream typeStr;

    switch (type->getTypeID()) {

        case llvm::Type::PointerTyID: {
            auto ptr = (llvm::PointerType *) type;
            // TODO `getPointerElementType` is actually deprecated and will be removed in the next version...
            std::string elementType = llvmTypeToCType(ctx, ptr->getNonOpaquePointerElementType());
            // TODO yet another naive instance that I might get away with for now...
            if (elementType == "char") // Make char pointers const-char
                elementType = "const char";
            typeStr << elementType << "*";
            return typeStr.str();
        }

        case llvm::Type::IntegerTyID: {
            auto intType = (llvm::IntegerType *) type;
            if (intType->getBitWidth() == boolWidth) {
                typeStr << "bool";
            } else if (intType->getBitWidth() == charWidth) {
                typeStr << "char";
            } else if (intType->getBitWidth() == shortWidth) {
                typeStr << "short";
            } else if (intType->getBitWidth() == intWidth) {
                typeStr << "int";
            } else if (intType->getBitWidth() == longWidth) {
                typeStr << "long";
            }

            break;
        }

        default:
            throw std::runtime_error("Invalid type");
    }

    return typeStr.str();
}
