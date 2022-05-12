//
// Created by charlie on 5/11/22.
//

#include "search_program_info_provider.h"

llvm::FunctionType *aotrc::compiler::SearchProgramInfoProvider::getFunctionType(llvm::LLVMContext &ctx) const {
    auto charPtrType = llvm::Type::getInt8PtrTy(ctx);
    auto sizeType = llvm::Type::getInt64Ty(ctx);
    auto boolType = llvm::Type::getInt1Ty(ctx);
    auto sizePtrTp = llvm::Type::getInt64PtrTy(ctx);

    // The two extra pointers are for where the string begins and ends
    return llvm::FunctionType::get(boolType, { charPtrType, sizeType, sizePtrTp, sizePtrTp }, false);
}

std::string aotrc::compiler::SearchProgramInfoProvider::getFunctionName(const std::string &regexLabel) const noexcept {
    return regexLabel + "_search";
}
