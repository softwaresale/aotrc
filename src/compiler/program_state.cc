//
// Created by charlie on 3/14/22.
//

#include "program_state.h"

aotrc::compiler::ProgramState::ProgramState(llvm::Function *programFunc)
        : context(programFunc->getContext())
        , irBuilder(this->context)
        , parentFunction(programFunc) {
    // Subject is the first argument
    this->subject = programFunc->args().begin();
    // Subject length is the second argument
    this->subjectLength = programFunc->args().begin() + 1;

    // Also, create the allocs for the two other variables

    // Get a reference to the entry block where we can actually build the allocs
    auto &funcBlocks = programFunc->getBasicBlockList();
    if (funcBlocks.empty()) {
        // If there is no starting basic block, then add one
        llvm::BasicBlock::Create(programFunc->getContext(), "entry_block", programFunc);
    }
    llvm::BasicBlock &entryBlock = programFunc->getEntryBlock();

    // Make a irBuilder and insert two allocas into the entry block, set to 0
    irBuilder.SetInsertPoint(&entryBlock);
    auto zeroLength = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx()), 0);
    auto zeroChar = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx()), 0);
    this->counter = irBuilder.CreateAlloca(llvm::Type::getInt64Ty(context), nullptr, "counter_var");
    irBuilder.CreateStore(zeroLength, this->counter);
    this->cursor = irBuilder.CreateAlloca(llvm::Type::getInt8Ty(context), nullptr, "cursor_var");
    irBuilder.CreateStore(zeroChar, this->cursor);

    // Build a state block for the start state
    auto startState = llvm::BasicBlock::Create(context, "STATE_0_START", parentFunction);
    this->stateBlocks[0] = startState;
    // build a link to the start state
    auto finalBranchInst = irBuilder.CreateBr(startState);

    // Add accept and reject blocks with returns
    this->acceptBlock = llvm::BasicBlock::Create(context, "ACCEPT_BLOCK", parentFunction);
    irBuilder.SetInsertPoint(acceptBlock);
    irBuilder.CreateRet(llvm::ConstantInt::getTrue(context));
    this->rejectBlock = llvm::BasicBlock::Create(context, "REJECT_BLOCK", parentFunction);
    irBuilder.SetInsertPoint(rejectBlock);
    irBuilder.CreateRet(llvm::ConstantInt::getFalse(context));

    // Leave builder at the entry block before the branch
    irBuilder.SetInsertPoint(finalBranchInst);
}

std::optional<llvm::BasicBlock *> aotrc::compiler::ProgramState::stateBlockAt(unsigned int state) const {
    try {
        auto block = this->stateBlocks.at(state);
        return {block};
    } catch (std::out_of_range &exe) {
        return {};
    }
}

llvm::BasicBlock *aotrc::compiler::ProgramState::stateBlock(unsigned int state) {
    // TODO just append these blocks for now, we can add them at a better location later
    // need to add a new block
    if (this->stateBlocks.find(state) == this->stateBlocks.end()) {
        this->stateBlocks[state] = llvm::BasicBlock::Create(this->context, "STATE_" + std::to_string(state) + "_START", this->parentFunction);
    }

    return this->stateBlocks[state];
}

aotrc::compiler::SubMatchProgramState::SubMatchProgramState(llvm::Function *programFunc)
: ProgramState(programFunc) {
    // builder is still set to before the branch
    // create the variable for match encountered and store false
    this->matchEncountered = this->builder().CreateAlloca(llvm::Type::getInt1Ty(ctx()));
    this->builder().CreateStore(llvm::ConstantInt::getFalse(ctx()), this->matchEncountered);
}

aotrc::compiler::SearchProgramState::SearchProgramState(llvm::Function *programFunc)
: SubMatchProgramState(programFunc)
, startPosParam(programFunc->getArg(2))
, endPosParam(programFunc->getArg(3)) {

    // Build location capturers
    this->startPos = this->builder().CreateAlloca(llvm::Type::getInt64Ty(ctx()), nullptr, "start_pos_var");
    auto zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx()), 0);
    this->endPos = this->builder().CreateAlloca(llvm::Type::getInt64Ty(ctx()), nullptr, "end_pos_var");
    this->builder().CreateStore(zero, this->startPos);
    this->builder().CreateStore(zero, this->endPos);

    // save the insertion position
    auto savedInsertBlock = this->builder().GetInsertBlock();
    auto savedInsertPoint = this->builder().GetInsertPoint();

    // Make a special block that stores the values
    auto nullVal = llvm::ConstantPointerNull::get(llvm::Type::getInt64PtrTy(this->ctx()));
    this->storePosBlock = llvm::BasicBlock::Create(this->ctx(), "STORE_POSITION", programFunc, this->getAcceptBlock());
    auto joinStoreEndBlock = llvm::BasicBlock::Create(this->ctx(), "COLLECT_JOIN_END", this->getParentFunction());
    this->builder().SetInsertPoint(storePosBlock);
    // conditionally store the start position
    llvm::Value *startPosIsNotNull = this->builder().CreateICmpNE(this->startPosParam, nullVal, "check_start_nonnull");
    auto storeStartBlock = llvm::BasicBlock::Create(this->ctx(), "STORE_START", programFunc);
    this->builder().SetInsertPoint(storeStartBlock);
    auto startVal = this->builder().CreateLoad(llvm::Type::getInt64Ty(this->ctx()), this->startPos, "start_position");
    this->builder().CreateStore(startVal, this->startPosParam);
    this->builder().CreateBr(joinStoreEndBlock);
    auto nopBlock = llvm::BasicBlock::Create(this->ctx(), "STORE_START_NOP_BLOCK", programFunc);
    this->builder().SetInsertPoint(nopBlock);
    this->builder().CreateBr(joinStoreEndBlock);
    this->builder().SetInsertPoint(this->storePosBlock);
    this->builder().CreateCondBr(startPosIsNotNull, storeStartBlock, nopBlock);

    // Conditionally store the end position
    this->builder().SetInsertPoint(joinStoreEndBlock);
    llvm::Value *endPosIsNotNull = this->builder().CreateICmpNE(this->endPosParam, nullVal, "check_end_nonnull");
    auto storeEndBlock = llvm::BasicBlock::Create(this->ctx(), "STORE_END", programFunc);
    this->builder().SetInsertPoint(storeEndBlock);
    auto endValue = this->builder().CreateLoad(llvm::Type::getInt64Ty(this->ctx()), this->getEndPos(), "end_position");
    this->builder().CreateStore(endValue, this->endPosParam);
    this->builder().CreateBr(this->getAcceptBlock());
    auto endNopBlock = llvm::BasicBlock::Create(this->ctx(), "STORE_END_NOP_BLOCK", programFunc);
    this->builder().SetInsertPoint(endNopBlock);
    this->builder().CreateBr(this->getAcceptBlock());
    this->builder().SetInsertPoint(joinStoreEndBlock);
    this->builder().CreateCondBr(endPosIsNotNull, storeEndBlock, endNopBlock);

    // reset the builder location
    this->builder().SetInsertPoint(savedInsertBlock, savedInsertPoint);
}
