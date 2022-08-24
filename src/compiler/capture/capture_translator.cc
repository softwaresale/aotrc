//
// Created by charlie on 8/21/22.
//

#include "capture_translator.h"

aotrc::compiler::CaptureTranslator::CaptureTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder,
                                                      llvm::Function *func)
                                                      : FullMatchTranslator(ctx, builder, func) {
    // store a reference to the group pointer
    this->symbolTable["groupPointer"] = func->getArg(2);
}

#if 0
llvm::Value *aotrc::compiler::CaptureTranslator::makeStoreArrayInst(const aotrc::compiler::InstructionPtr &inst) {
    auto storeArrayInst = dynamic_cast<StoreArrayInstruction *>(inst.get());
    // Load the value that we want to store
    auto sourceVar = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at(storeArrayInst->sourceVar));
    auto sourceValue = this->builder.CreateLoad(sourceVar->getAllocatedType(), sourceVar);
    auto destArray = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at(storeArrayInst->destVar));
    auto destTp = destArray->getAllocatedType();

    std::vector<llvm::Value *> arrayIndex;
    std::transform(storeArrayInst->index.cbegin(), storeArrayInst->index.cend(), std::back_inserter(arrayIndex),
                   [this](const unsigned int &idx) { return llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(this->ctx), idx); });
    arrayIndex.insert(arrayIndex.begin(), llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(ctx), 0));

    auto destPtr = this->builder.CreateGEP(destTp, destArray, arrayIndex);
    this->builder.CreateStore(sourceValue, destPtr);

    return nullptr;
}
#endif

llvm::Value *aotrc::compiler::CaptureTranslator::makeAcceptInst(const aotrc::compiler::InstructionPtr &inst) {

    auto acceptBlock = llvm::dyn_cast<llvm::BasicBlock>(this->symbolTable.at("accept_block"));
    auto captureInfoStructPtr = llvm::dyn_cast<llvm::Argument>(this->symbolTable.at("groupPointer"));
    auto captureInfoStructTp = llvm::dyn_cast<llvm::PointerType>(captureInfoStructPtr->getType())->getPointerElementType();

    // Build out the code to store all the capture info
    auto [backupBlock, backupInst] = this->storeInsertPoint();
    auto storeCaptureInfoBlock = llvm::BasicBlock::Create(ctx, "STORE_CAPTURE_INFO", this->function, acceptBlock);
    this->builder.SetInsertPoint(storeCaptureInfoBlock);
    // For each group, copy over the value
    auto groupsAlloca = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at("groups"));
    auto groupsType = groupsAlloca->getAllocatedType();
    llvm::Value *zero = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(ctx), 0);
    llvm::Value *endIdx = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(ctx), 1);
    auto sizeType = llvm::IntegerType::getInt64Ty(ctx);
    for (unsigned int element = 0; element < groupsType->getArrayNumElements(); element++) {
        llvm::Value *groupIdx = llvm::ConstantInt::get(sizeType, element);
        auto startValuePtr = this->builder.CreateGEP(groupsType, groupsAlloca, {zero, groupIdx, zero}, "start_val_ptr");
        auto endValuePtr = this->builder.CreateGEP(groupsType, groupsAlloca, {zero, groupIdx, endIdx}, "end_val_ptr");

        auto structBody = this->builder.CreateStructGEP(captureInfoStructTp, captureInfoStructPtr, element, "struct_group_ptr");
        auto structStartPtr = this->builder.CreateGEP(structBody->getType()->getPointerElementType(), structBody, {zero, zero}, "struct_group_start_ptr");
        auto structEndPtr = this->builder.CreateGEP(structBody->getType()->getPointerElementType(), structBody, {zero, endIdx}, "struct_group_end_ptr");

        auto startValue = this->builder.CreateLoad(startValuePtr->getType()->getPointerElementType(), startValuePtr, "start_val");
        auto endValue = this->builder.CreateLoad(endValuePtr->getType()->getPointerElementType(), endValuePtr, "end_val");
        this->builder.CreateStore(startValue, structStartPtr);
        this->builder.CreateStore(endValue, structEndPtr);
    }
    this->builder.CreateBr(acceptBlock);

    // Return to where we were. Conditionally go to the store capture info block if the pointer is non-null
    this->builder.SetInsertPoint(backupBlock, backupInst);
    auto captureStructPointerTp = llvm::dyn_cast<llvm::PointerType>(captureInfoStructPtr->getType());
    auto nullVal = llvm::ConstantPointerNull::get(captureStructPointerTp);
    auto isNonNull = this->builder.CreateICmpNE(captureInfoStructPtr, nullVal);
    this->builder.CreateCondBr(isNonNull, storeCaptureInfoBlock, acceptBlock);

    return nullptr;
}

llvm::Value *aotrc::compiler::CaptureTranslator::makeEntryBlock(
        std::vector<std::reference_wrapper<const InstructionPtr>> &&setupInstructions) {
    auto block = InstructionTranslator::makeEntryBlock(std::move(setupInstructions));
    auto infoStructPtrType = llvm::dyn_cast<llvm::PointerType>(this->function->getArg(2)->getType());
    // Alloca a group info
    auto capInfo = this->builder.CreateAlloca(infoStructPtrType->getPointerElementType(), nullptr, "groups");
    this->symbolTable["groups"] = capInfo;
    return block;
}
