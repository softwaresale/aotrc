//
// Created by charlie on 3/13/22.
//

#include "instruction.h"

std::string aotrc::compiler::ConsumeInstruction::str() const noexcept {
    return "CONSUME";
}

std::string aotrc::compiler::CheckEndInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "CHECK_END";
    if (this->onTrueInst)
        ss << " -> " << this->onTrueInst->str();

    return ss.str();
}

std::string aotrc::compiler::TestInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "TEST {";
    auto it = this->ranges.begin();
    for (; it != this->ranges.end() - 1; ++it) {
        ss << *it << ',';
    }
    ss << *it << '}';
    return ss.str();
}

std::string aotrc::compiler::GotoInstruction::str() const noexcept {
    std::stringstream ss;
    ss << "GOTO";
    if (this->testInst) {
        ss << ' ' << this->testInst->str();
    }
    ss << ' ' << this->destId;

    return ss.str();
}

std::string aotrc::compiler::AcceptInstruction::str() const noexcept {
    return "ACCEPT";
}

std::string aotrc::compiler::RejectInstruction::str() const noexcept {
    return "REJECT";
}

std::string aotrc::compiler::StartStateInstruction::str() const noexcept {
    return "START " + std::to_string(this->id);
}

/*--BUILD-FUNCTIONS--*/

llvm::Value *aotrc::compiler::StartStateInstruction::build(std::unique_ptr<ProgramState> &state) {
    // Optionally create a new state block for the given id
    auto stateBlockOpt = state->stateBlock(this->id);
    // Set the insertion point here
    state->builder().SetInsertPoint(stateBlockOpt);
    return nullptr;
}

llvm::Value *aotrc::compiler::ConsumeInstruction::build(std::unique_ptr<ProgramState> &state) {
    // First, load the counter value
    auto counterValue = state->builder().CreateLoad(llvm::Type::getInt64Ty(state->ctx()), state->getCounter(), "counter_value");
    // Next, create a GEP to get a pointer to where we want to deref the subject string
    auto cursorValuePtr = state->builder().CreateGEP(llvm::Type::getInt8Ty(state->ctx()), state->getSubject(), counterValue, "cursor_value_ptr");
    // Next, deref the cursor value ptr
    auto cursorValue = state->builder().CreateLoad(llvm::Type::getInt8Ty(state->ctx()), cursorValuePtr, "cursor_value");
    // Next, store the cursor value into the cursor variable
    state->builder().CreateStore(cursorValue, state->getCursor());

    // The cursor value is stored, now increment the counter variable
    auto one = llvm::ConstantInt::get(llvm::Type::getInt64Ty(state->ctx()), 1);
    auto incrementedCounterValue = state->builder().CreateAdd(counterValue, one);
    // Store the incremented result
    state->builder().CreateStore(incrementedCounterValue, state->getCounter());

    // fin
    return nullptr;
}

llvm::Value *aotrc::compiler::CheckEndInstruction::build(std::unique_ptr<ProgramState> &state) {
    // Insert two basic blocks after the current insertion point
    llvm::BasicBlock *currentInsertionBlock = state->builder().GetInsertBlock();
    // TODO try adding the follow up blocks in a better location. For now, I just want things to compile...

    // Create a then block, place it before the successor block (and therefore after the current insertion block
    auto thenBlock = llvm::BasicBlock::Create(state->ctx(), "CHECK_END_ON_TRUE", state->getParentFunction());
    // build the on_true inst into the then block
    state->builder().SetInsertPoint(thenBlock);
    this->onTrueInst->build(state);
    // insert the then block after the old current insertion point block

    // create an else block and add it before the successor block
    auto elseBlock = llvm::BasicBlock::Create(state->ctx(), "CHECK_END_FALLTHROUGH", state->getParentFunction());

    // Now that we have destination blocks, actually build the conditional
    // 0. reset the insert point to be the original insertion point
    state->builder().SetInsertPoint(currentInsertionBlock);
    // 1. load the counter value
    auto counterValue = state->builder().CreateLoad(llvm::Type::getInt64Ty(state->ctx()), state->getCounter(), "counter_value");
    // 2. compare this to the subject length
    auto atEnd = state->builder().CreateICmpEQ(counterValue, state->getSubjectLength(), "is_at_end");
    // 3. Branch
    state->builder().CreateCondBr(atEnd, thenBlock, elseBlock);

    // leave the insertion point on the else block
    state->builder().SetInsertPoint(elseBlock);

    // Return this block, even though it may not be used
    return elseBlock;
}

llvm::Value *aotrc::compiler::TestInstruction::build(std::unique_ptr<ProgramState> &state) {

    // Load the cursor value
    auto cursorValue = state->builder().CreateLoad(llvm::Type::getInt8Ty(state->ctx()), state->getCursor());
    // Begin checking ranges
    std::vector<llvm::Value *> eachRangeCheck(this->ranges.size());
    unsigned int idx = 0;
    for (const auto &range : this->ranges) {
        // Create a check value
        llvm::Value *rangeCheck;
        if (range.lower == range.upper) {
            auto rangeValue = llvm::ConstantInt::get(llvm::Type::getInt8Ty(state->ctx()), range.lower);
            rangeCheck = state->builder().CreateICmpEQ(rangeValue, cursorValue);
        } else {
            auto rangeLower = llvm::ConstantInt::get(llvm::Type::getInt8Ty(state->ctx()), range.lower);
            auto rangeUpper = llvm::ConstantInt::get(llvm::Type::getInt8Ty(state->ctx()), range.upper);

            // Check lower bound
            auto lowerCheck = state->builder().CreateICmpUGE(cursorValue, rangeLower);
            auto upperCheck = state->builder().CreateICmpULE(cursorValue, rangeUpper);

            // And them together
            rangeCheck = state->builder().CreateAnd(lowerCheck, upperCheck);
        }
        eachRangeCheck[idx++] = rangeCheck;
    }

    // Now that we have all the range checks, or them all together to see if the edge fits
    return state->builder().CreateOr(eachRangeCheck);
}

llvm::Value *aotrc::compiler::GotoInstruction::build(std::unique_ptr<ProgramState> &state) {

    llvm::BasicBlock *fallthrough = nullptr;

    // Start by getting the destination state
    llvm::BasicBlock *destStateBlock = state->stateBlock(this->destId);

    if (this->testInst) {
        // Conditional
        // Build out the test instruction
        auto testValue = this->testInst->build(state);
        // Add a fallthrough block
        // Add it after the current insertion point
        // TODO try to add this at a better location
        fallthrough = llvm::BasicBlock::Create(state->ctx(), "GOTO_FALLTHROUGH", state->getParentFunction());
        // Create the branch
        state->builder().CreateCondBr(testValue, destStateBlock, fallthrough);
        // Set the insertion point at the fall through block
        state->builder().SetInsertPoint(fallthrough);
    } else {
        // unconditional
        state->builder().CreateBr(destStateBlock);
    }

    return fallthrough;
}

llvm::Value *aotrc::compiler::AcceptInstruction::build(std::unique_ptr<ProgramState> &state) {
    state->builder().CreateBr(state->getAcceptBlock());
    return nullptr;
}

llvm::Value *aotrc::compiler::RejectInstruction::build(std::unique_ptr<ProgramState> &state) {
    state->builder().CreateBr(state->getRejectBlock());
    return nullptr;
}
