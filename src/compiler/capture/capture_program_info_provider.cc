//
// Created by charlie on 8/21/22.
//

#include "capture_program_info_provider.h"

llvm::FunctionType *aotrc::compiler::CaptureProgramInfoProvider::getFunctionType(llvm::LLVMContext &ctx) const {
    auto charPtrType = llvm::Type::getInt8PtrTy(ctx);
    auto sizeType = llvm::Type::getInt64Ty(ctx);
    auto boolType = llvm::Type::getInt1Ty(ctx);
    auto infoStructPtr = llvm::PointerType::get(this->captureInfoStruct, 0);

    return llvm::FunctionType::get(boolType, { charPtrType, sizeType, infoStructPtr }, false);
}

std::string aotrc::compiler::CaptureProgramInfoProvider::getFunctionName(const std::string &regexLabel) const noexcept {
    return regexLabel + "_captures";
}
