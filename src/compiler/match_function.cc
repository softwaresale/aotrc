//
// Created by charlie on 12/4/21.
//

#include "match_function.h"

aotrc::compiler::MatchFunction::MatchFunction(aotrc::fa::DFA &&dfa,
                                              const std::string &label,
                                              std::shared_ptr<llvm::Module> &parentModule,
                                              const std::shared_ptr<CompilerContext> &ctx)
                                              : ctx(ctx)
                                              , dfa(dfa) {
    // match function type
    auto stringType = llvm::Type::getInt8PtrTy(ctx->context());
    auto lengthType = llvm::Type::getInt32Ty(ctx->context());
    auto matchFunctionType = llvm::FunctionType::get(llvm::Type::getInt1Ty(ctx->context()), {stringType, lengthType}, false);
    // Create the function
    std::string functionName = "match_" + label;
    parentModule->getOrInsertFunction(functionName, matchFunctionType);
    this->matchFunction = parentModule->getFunction(functionName);

    // Build out the initial blocks
    this->initialBlock = llvm::BasicBlock::Create(ctx->context(), "setup", this->matchFunction);
    ctx->builder().SetInsertPoint(this->initialBlock);
    this->counterVar = ctx->builder().CreateAlloca(llvm::Type::getInt32Ty(ctx->context()), 0, nullptr, "counter");
    ctx->builder().CreateStore(llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx->context()), 0), this->counterVar);

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
}

llvm::Value *aotrc::compiler::MatchFunction::getPatternLengthArg() {
    return this->matchFunction->getArg(1);
}

llvm::Value *aotrc::compiler::MatchFunction::getPatternArg() {
    return this->matchFunction->getArg(0);
}

void aotrc::compiler::MatchFunction::build() {
    // construct a state graph and build it out
    StateGraph stateGraph(this->dfa, this->ctx, this);
    auto firstStateBlock = stateGraph.build();

    // append this block after the initial block
    auto firstBasicBlock = this->matchFunction->begin();
    this->matchFunction->getBasicBlockList().insertAfter(firstBasicBlock, firstStateBlock);

    // fin
}
