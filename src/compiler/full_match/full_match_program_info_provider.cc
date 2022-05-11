//
// Created by charlie on 5/11/22.
//

#include "full_match_program_info_provider.h"

llvm::FunctionType *aotrc::compiler::FullMatchProgramInfoProvider::getFunctionType(llvm::LLVMContext &ctx) const {
    auto charPtrType = llvm::Type::getInt8PtrTy(ctx);
    auto sizeType = llvm::Type::getInt64Ty(ctx);
    auto boolType = llvm::Type::getInt1Ty(ctx);

    return llvm::FunctionType::get(boolType, { charPtrType, sizeType }, false);
}

std::string
aotrc::compiler::FullMatchProgramInfoProvider::getFunctionName(const std::string &regexLabel) const noexcept {
    return regexLabel + "_full_match";
}
