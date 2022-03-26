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

    // Make a irBuilder and insert two allocas into the entry block
    irBuilder.SetInsertPoint(&entryBlock);
    this->counter = irBuilder.CreateAlloca(llvm::Type::getInt64Ty(context));
    this->cursor = irBuilder.CreateAlloca(llvm::Type::getInt8Ty(context));

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
