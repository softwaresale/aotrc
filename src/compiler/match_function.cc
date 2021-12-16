//
// Created by charlie on 12/4/21.
//

#include "match_function.h"

aotrc::compiler::MatchFunction::MatchFunction(aotrc::fa::DFA &&dfa,
                                              const std::string &label,
                                              bool isSubMatch,
                                              std::shared_ptr<llvm::Module> &parentModule,
                                              const std::shared_ptr<CompilerContext> &ctx)
                                              : ctx(ctx)
                                              , dfa(dfa)
                                              , isSubMatch(isSubMatch) {
    // match function type
    auto stringType = llvm::Type::getInt8PtrTy(ctx->context());
    auto lengthType = llvm::Type::getInt32Ty(ctx->context());
    auto matchFunctionType = llvm::FunctionType::get(llvm::Type::getInt1Ty(ctx->context()), {stringType, lengthType}, false);
    // Create the function
    std::string functionName;
    if (this->isSubMatch) {
        functionName = "match_" + label;
    } else {
        functionName = "fullmatch_" + label;
    }
    parentModule->getOrInsertFunction(functionName, matchFunctionType);
    this->matchFunction = parentModule->getFunction(functionName);

    // Build out the initial blocks
    this->initialBlock = llvm::BasicBlock::Create(ctx->context(), "setup", this->matchFunction);
    ctx->builder().SetInsertPoint(this->initialBlock);
    this->counterVar = ctx->builder().CreateAlloca(llvm::Type::getInt32Ty(ctx->context()), 0, nullptr, "counter");
    ctx->builder().CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx->context()), 0), this->counterVar);

    // Add the accept block
    this->acceptBlock = llvm::BasicBlock::Create(ctx->context(), "accept", this->matchFunction);
    ctx->builder().SetInsertPoint(this->acceptBlock);
    // default return true
    ctx->builder().CreateRet(llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx->context()), 1));

    // Add the reject block
    this->rejectBlock = llvm::BasicBlock::Create(ctx->context(), "reject", this->matchFunction);
    ctx->builder().SetInsertPoint(this->rejectBlock);
    // default return true
    ctx->builder().CreateRet(llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx->context()), 0));

    // Create state blocks for everything
    for (unsigned int state = 0; state < this->dfa.stateCount(); state++) {
        // Create a new state
        MatchFunctionState matchState(state, dfa.isAcceptState(state), dfa.isLeaf(state), this->matchFunction, ctx);
        // Add it to states
        this->states[state] = matchState;
    }
}

llvm::Value *aotrc::compiler::MatchFunction::getPatternLengthArg() {
    return this->matchFunction->getArg(1);
}

llvm::Value *aotrc::compiler::MatchFunction::getPatternArg() {
    return this->matchFunction->getArg(0);
}

void aotrc::compiler::MatchFunction::build() {
    // Build out all the state stuff
    for (unsigned int state = 0; state < this->dfa.stateCount(); state++) {
        // Build out the initial state
        this->states[state].buildInitialState(this->counterVar, this->getPatternLengthArg(), this->acceptBlock, this->rejectBlock);

        // Build out the compute block if there is one
        if (!this->states[state].leaf()) {
            this->states[state].buildComputeBlock(this->counterVar, this->getPatternArg());
        }

        // for each outgoing edge, build an edge
        for (const auto &transition : this->dfa.edgesForState(state)) {
            unsigned int destState = transition.first;
            auto edge = transition.second;

            // Build outgoing edges everywhere
            llvm::BasicBlock *fallbackBlock = this->rejectBlock;
            if (this->isSubMatch) {
                // If we are submatching, then we don't want to go to the reject block. Instead, we want to go back to
                // the start state
                fallbackBlock = this->states[dfa.getStartState()].getInitialBlock();
            }
            this->states[state].buildEdgeTo(this->matchFunction, this->states[destState], edge, fallbackBlock);
        }

        // Build the link to the first edge if it's not a leaf and therefore has a compute block and a first edge
        if (!this->states[state].leaf()) {
            this->states[state].buildLinkToFirstEdge();
        }
    }

    // Build a link from the initial block to the start state
    ctx->builder().SetInsertPoint(this->initialBlock);
    ctx->builder().CreateBr(this->states[0].getInitialBlock());
}
