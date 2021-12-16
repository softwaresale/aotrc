//
// Created by charlie on 12/5/21.
//

#include "match_function_state.h"

aotrc::compiler::MatchFunctionState::MatchFunctionState(bool submatch)
: computeBlock(nullptr)
, initialBlock(nullptr)
, isLeaf(false)
, isAccept(false)
, counterVal(nullptr)
, cursorVal(nullptr)
, isSubMatch(submatch) {
}

aotrc::compiler::MatchFunctionState::MatchFunctionState(unsigned int state,
                                                        bool isAccept, bool isLeaf,
                                                        llvm::Function *parentFunc,
                                                        const std::shared_ptr<CompilerContext> &ctx,
                                                        bool isSubMatch)
                                                        : ctx(ctx)
                                                        , stateLabel("STATE" + std::to_string(state))
                                                        , isAccept(isAccept)
                                                        , isLeaf(isLeaf)
                                                        , computeBlock(nullptr)
                                                        , initialBlock(nullptr)
                                                        , counterVal(nullptr)
                                                        , cursorVal(nullptr)
                                                        , isSubMatch(isSubMatch) {

    // For now, just create the initial block and a compute block if it's not a leaf
    this->initialBlock = llvm::BasicBlock::Create(ctx->context(), this->stateLabel + "_INITIAL", parentFunc);
    this->computeBlock = !isLeaf ? llvm::BasicBlock::Create(ctx->context(), this->stateLabel + "_COMPUTE", parentFunc) : nullptr;
}

void aotrc::compiler::MatchFunctionState::buildInitialState(llvm::Value *counterVar, llvm::Value *lengthArg, llvm::BasicBlock *accept, llvm::BasicBlock *reject) {
    // Set the position
    ctx->builder().SetInsertPoint(this->initialBlock);

    // Build the checks
    this->counterVal = ctx->builder().CreateLoad(llvm::Type::getInt32Ty(ctx->context()), counterVar, "counterVal");

    auto atEnd = ctx->builder().CreateICmpEQ(counterVal, lengthArg);
    llvm::BasicBlock *thenBlock = this->isAccept ? accept : reject;
    llvm::BasicBlock *elseBlock = this->isLeaf ? reject : this->computeBlock;

    ctx->builder().CreateCondBr(atEnd, thenBlock, elseBlock);
}

void aotrc::compiler::MatchFunctionState::buildComputeBlock(llvm::Value *counterVar, llvm::Value *inputTextPtr) {
    ctx->builder().SetInsertPoint(this->computeBlock);

    // calculate the pointed at value
    auto cursorValPtr = ctx->builder().CreateGEP(llvm::Type::getInt8Ty(ctx->context()), inputTextPtr, this->counterVal, "cursorValPtr");
    this->cursorVal = ctx->builder().CreateLoad(llvm::Type::getInt8Ty(ctx->context()), cursorValPtr, "cursorVal");

    // increment the cursor
    auto incrementedCounter = ctx->builder().CreateAdd(this->counterVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx->context()), 1));
    ctx->builder().CreateStore(incrementedCounter, counterVar);

    // NOTE: we still need to branch unconditionally to the first edge block
}

void
aotrc::compiler::MatchFunctionState::buildEdgeTo(llvm::Function *parent, const aotrc::compiler::MatchFunctionState &destState, const aotrc::fa::Edge &edge, llvm::BasicBlock *reject) {
    // Create a new edge block and add it in after our compute block
    llvm::BasicBlock *edgeBlock = llvm::BasicBlock::Create(ctx->context(), this->stateLabel + "_EDGE" + std::to_string(this->edgeBlocks.size()), parent);
    ctx->builder().SetInsertPoint(edgeBlock);

    // If we have other edge blocks, go to the back edge block. Otherwise, go to reject
    auto fallbackBlock = this->edgeBlocks.empty() ? reject : this->edgeBlocks.back();

    // Build out stuff with the edge
    std::vector<llvm::Value *> rangeComparisons;
    for (const auto &range : edge.getRanges()) {
        llvm::Value *rangeCmp;
        if (range.lower == range.upper) {
            rangeCmp = ctx->builder().CreateICmpEQ(this->cursorVal, llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx->context()), range.lower));
        } else {
            auto isAboveLower = ctx->builder().CreateICmpUGE(this->cursorVal, llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx->context()), range.lower));
            auto isBelowUpper = ctx->builder().CreateICmpULE(this->cursorVal, llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx->context()), range.upper));
            rangeCmp = ctx->builder().CreateAnd(isBelowUpper, isAboveLower);
        }
        rangeComparisons.push_back(rangeCmp);
    }

    llvm::Value *canTakeEdge;
    if (rangeComparisons.size() > 1) {
        canTakeEdge = ctx->builder().CreateOr(rangeComparisons);
    } else {
        canTakeEdge = rangeComparisons[0];
    }

    // Create the branch
    ctx->builder().CreateCondBr(canTakeEdge, destState.initialBlock, fallbackBlock);

    // Add this block to state blocks
    this->edgeBlocks.push_back(edgeBlock);
}

void aotrc::compiler::MatchFunctionState::buildLinkToFirstEdge() {
    ctx->builder().SetInsertPoint(this->computeBlock);
    // Branch to the last block
    ctx->builder().CreateBr(this->edgeBlocks.back());
}
