//
// Created by charlie on 5/11/22.
//

#include "search_translator.h"

aotrc::compiler::SearchTranslator::SearchTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder,
                                                    llvm::Function *func)
                                                    : FullMatchTranslator(ctx, builder, func) {

    // Add the last two to the symbol table
    this->symbolTable["start_pos_ptr"] = func->getArg(2);
    this->symbolTable["end_pos_ptr"] = func->getArg(3);
}

llvm::Value *aotrc::compiler::SearchTranslator::makeGoToInst(const aotrc::compiler::InstructionPtr &inst) {
    auto goToInst = dynamic_cast<GoToInstruction *>(inst.get());

    // Get the destined block
    auto destBlock = llvm::dyn_cast<llvm::BasicBlock>(this->symbolTable.at(getStateBlockLabel(goToInst->destId)));

    if (goToInst->testInstruction) {
        // If there is a test instruction, build it out, make a branch, and return the collect block
        auto canGoTo = this->makeInstruction(goToInst->testInstruction);

        // backup
        auto backup = this->storeInsertPoint();

        llvm::BasicBlock *onFalse, *collect;
        collect = llvm::BasicBlock::Create(ctx, "GO_TO_INST_COLLECT", function);

        onFalse = llvm::BasicBlock::Create(ctx, "GO_TO_INST_ON_FALSE", function, collect);
        builder.SetInsertPoint(onFalse);
        builder.CreateBr(collect);

        // Build a cond br
        this->restoreInsertPoint(backup);
        // Increment the end position variable
        auto endPosVar = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at("end_pos"));
        auto endPosVal = this->builder.CreateLoad(endPosVar->getAllocatedType(), endPosVar, "end_pos_val");
        auto updated = this->builder.CreateAdd(endPosVal, llvm::ConstantInt::get(endPosVar->getAllocatedType(), 1), "incremented_end_pos_val");
        this->builder.CreateStore(updated, endPosVar);

        // Actually do the branch
        builder.CreateCondBr(canGoTo, destBlock, onFalse);

        builder.SetInsertPoint(collect);
        return collect;

    } else {
        // If there is no condition, then just branch
        builder.CreateBr(destBlock);
        return nullptr;
    }
}

llvm::Value *aotrc::compiler::SearchTranslator::makeAcceptInst(const aotrc::compiler::InstructionPtr &inst) {

    // First, create a new block and branch to it
    auto startStoringValuesBlock = llvm::BasicBlock::Create(ctx, "SEARCH_ACCEPT_STORE_POSES", function);
    this->builder.CreateBr(startStoringValuesBlock);
    this->builder.SetInsertPoint(startStoringValuesBlock);

    // Get the variables that we are interested in
    auto startPos = this->symbolTable.at("start_pos_ptr");
    auto endPos = this->symbolTable.at("end_pos_ptr");
    auto startPosVar = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at("start_pos"));
    auto endPosVar = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at("end_pos"));

    // First, create a conditional store for start pos
    this->createNonNullStore(startPosVar, startPos);
    // next, create a conditional store for end pos
    this->createNonNullStore(endPosVar, endPos);

    // After the variables are stored, branch to accept
    return FullMatchTranslator::makeAcceptInst(inst);
}

llvm::Value *aotrc::compiler::SearchTranslator::makeStoreVarInst(const aotrc::compiler::InstructionPtr &inst) {
    auto storeVarInst = dynamic_cast<StoreVarInstruction *>(inst.get());
    // First, figure out which variable we are storing to
    auto destVar = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at(storeVarInst->destVar));
    // Load the value from the source variable
    auto sourceVar = llvm::dyn_cast<llvm::AllocaInst>(this->symbolTable.at(storeVarInst->sourceVar));
    // Load the value, which is the value we want to store
    llvm::Value *valToStore = this->builder.CreateLoad(sourceVar->getAllocatedType(), sourceVar);

    // Now, actually store the value into the destination
    this->builder.CreateStore(valToStore, destVar);

    // Done
    return nullptr;
}

llvm::BasicBlock *
aotrc::compiler::SearchTranslator::createStoreBlock(llvm::AllocaInst *valueToStore, llvm::Value *dest, llvm::BasicBlock *collectBlock) {
    auto backup = this->storeInsertPoint();

    // Fisrt, create a new basic block
    auto storeBlock = llvm::BasicBlock::Create(ctx, "STORE_VALUE_BLOCK", function);
    this->builder.SetInsertPoint(storeBlock);

    // Load the value
    auto storedValue = this->builder.CreateLoad(valueToStore->getAllocatedType(), valueToStore, "contained_value");
    // Store the value to destination
    this->builder.CreateStore(storedValue, dest);
    // Jump to the collect block
    this->builder.CreateBr(collectBlock);

    this->restoreInsertPoint(backup);

    return storeBlock;
}

llvm::Value *aotrc::compiler::SearchTranslator::buildNonNullCheck(llvm::Value *valueToCheck) {
    // literal null
    static auto nullVal = llvm::ConstantPointerNull::get(llvm::Type::getInt64PtrTy(ctx));

    // Return a boolean value for if the given value is null
    return this->builder.CreateICmpNE(nullVal, valueToCheck, valueToCheck->getName() + "_is_non_null");
}

llvm::BasicBlock *
aotrc::compiler::SearchTranslator::createNonNullStore(llvm::AllocaInst *varToStore, llvm::Value *potentialDest) {
    // First, make all the blocks
    auto collectBlock = llvm::BasicBlock::Create(ctx, "NON_NULL_COLLECT", function);
    auto onFalseBlock = llvm::BasicBlock::Create(ctx, "NON_NULL_STORE_FALSE", function, collectBlock);
    auto onTrueBlock = this->createStoreBlock(varToStore, potentialDest, collectBlock);

    // build the null check
    auto destIsNonNull = this->buildNonNullCheck(potentialDest);

    // Actually create the branch
    this->builder.CreateCondBr(destIsNonNull, onTrueBlock, onFalseBlock);

    // Set the branch in onFalse to point at collect
    this->builder.SetInsertPoint(onFalseBlock);
    this->builder.CreateBr(collectBlock);

    // Position on collect block
    this->builder.SetInsertPoint(collectBlock);

    return collectBlock;
}
