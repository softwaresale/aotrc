//
// Created by charlie on 5/11/22.
//

#include "full_match_translator.h"

aotrc::compiler::FullMatchTranslator::FullMatchTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder,
                                                          llvm::Function *func)
                                                          : InstructionTranslator(ctx, builder, func) {

    // Load function parameters into the symbol table
    this->symbolTable["subject"] = func->getArg(0);
    this->symbolTable["subject_len"] = func->getArg(1);
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeDeclareVarInst(const aotrc::compiler::InstructionPtr &inst) {
    // build an alloca instance
    auto declareVarInst = dynamic_cast<DeclareVarInstruction *>(inst.get());

    // Get the type and initial value of the variable
    llvm::Type *varType;
    llvm::Value *initialValue;
    if (declareVarInst->varType == VariableType::BOOL) {
        varType = llvm::Type::getInt1Ty(this->ctx);
        initialValue = llvm::ConstantInt::get(varType, std::get<bool>(declareVarInst->initialValue));
    } else if (declareVarInst->varType == VariableType::CHAR) {
        varType = llvm::Type::getInt8Ty(ctx);
        initialValue = llvm::ConstantInt::get(varType, std::get<char>(declareVarInst->initialValue));
    } else {
        varType = llvm::Type::getInt64Ty(ctx);
        initialValue = llvm::ConstantInt::get(varType, std::get<size_t>(declareVarInst->initialValue));
    }

    // Build an alloca instance
    auto varInst = builder.CreateAlloca(varType, nullptr, declareVarInst->name);
    builder.CreateStore(initialValue, varInst);

    // Add variable to the symbol table
    this->symbolTable[declareVarInst->name] = varInst;

    // Done
    return varInst;
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeStartStateInst(const aotrc::compiler::InstructionPtr &inst) {
    auto startStateInst = dynamic_cast<StartStateInstruction *>(inst.get());
    // Create a new blockValue and place it in the symbol table if it hasn't been already
    auto stateLabel = startStateInst->getStateLabel();
    llvm::Value *blockValue;
    if (this->symbolTable.count(stateLabel) == 0) {
        blockValue = llvm::BasicBlock::Create(this->ctx, stateLabel, this->function, nullptr);
        this->symbolTable[stateLabel] = blockValue;
    } else {
        blockValue = this->symbolTable.at(stateLabel);
    }

    auto block = static_cast<llvm::BasicBlock *>(blockValue);
    builder.SetInsertPoint(block);

    return blockValue;
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeConsumeInst(const aotrc::compiler::InstructionPtr &inst) {
    // Load the value of counter
    auto counterVar = this->symbolTable.at("counter");
    auto subjectVar = this->symbolTable.at("subject");
    auto cursorVar = this->symbolTable.at("cursor");
    auto counterVal = builder.CreateLoad(llvm::Type::getInt64Ty(ctx), counterVar, "counter_val");
    // Retrieve a character from the subject string pointed at by counter
    auto cursorPtr = builder.CreateGEP(llvm::Type::getInt8Ty(ctx), subjectVar, counterVal, "cursor_ptr");
    // Load the cursor value, store it in cursor
    auto cursorVal = builder.CreateLoad(llvm::Type::getInt8Ty(ctx), cursorPtr, "cursor_val");
    builder.CreateStore(cursorVal, cursorVar);
    // Increment the counter value and store it back in the counter variable
    auto updatedCounterVal = builder.CreateAdd(llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 1), counterVal);
    builder.CreateStore(updatedCounterVal, counterVar);
    // Cursor is set up and counter is incremented. Done

    return cursorVal;
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeCheckEndInst(const aotrc::compiler::InstructionPtr &inst) {
    auto checkEndInst = dynamic_cast<CheckEndInstruction *>(inst.get());
    // Load the counter var
    auto counterVar = this->symbolTable.at("counter");
    auto subjectLen = this->symbolTable.at("subject_len");
    auto counterVal = builder.CreateLoad(llvm::Type::getInt64Ty(ctx), counterVar, "counter_val");
    // Compare the counter val to the subject length
    auto isAtEnd = builder.CreateICmpEQ(counterVal, subjectLen, "is_at_end");

    // Back up the builder
    auto backupBlock = builder.GetInsertBlock();
    auto backupPoint = builder.GetInsertPoint();

    // Make three blocks: on true, on false, and collect
    llvm::BasicBlock *onTrue, *onFalse, *collect;

    // Collect: starts the next edge/batch of instructions
    collect = llvm::BasicBlock::Create(ctx, "CHECK_END_COLLECT", function);

    // On False: just jump to the collect block
    onFalse = llvm::BasicBlock::Create(ctx, "CHECK_END_ON_FALSE", function, collect);
    builder.SetInsertPoint(onFalse);
    // Just branch unconditionally to collect
    builder.CreateBr(collect);

    // On true: execute if we are at the end
    onTrue = llvm::BasicBlock::Create(ctx, "CHECK_END_ON_TRUE", function, onFalse);
    builder.SetInsertPoint(onTrue);
    // Build the on true instruction, which will likely branch away
    this->makeInstruction(checkEndInst->onTrueInst);

    // Set insert point back at the backup point
    builder.SetInsertPoint(backupBlock, backupPoint);
    // Create a branch
    builder.CreateCondBr(isAtEnd, onTrue, onFalse);

    // Set the insertion point on the collect block
    builder.SetInsertPoint(collect);

    // Return the collect block if it's desired
    return collect;
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeTestEdgeInst(const aotrc::compiler::InstructionPtr &inst) {
    auto testEdgeInst = dynamic_cast<TestEdgeInstruction *>(inst.get());
    // Get the character to be tested
    auto cursorVar = this->symbolTable.at("cursor");
    auto cursorVal = builder.CreateLoad(llvm::Type::getInt8Ty(ctx), cursorVar, "cursor_val");

    // If there is one range, then return a simple thing
    auto ranges = testEdgeInst->edgeToTest.getRanges();
    std::vector<llvm::Value *> fitsARange(ranges.size());
    int idx = 0;
    for (const auto &range : ranges) {
        auto lowerVal = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx), range.lower);
        llvm::Value *rangeTest;

        if (range.lower == range.upper) {
            rangeTest = builder.CreateICmpEQ(lowerVal, cursorVal, "is_same");
        } else {
            auto upperVal = llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx), range.upper);

            // See if it fits between the ranges
            auto aboveLower = builder.CreateICmpUGE(cursorVal, lowerVal, "above_lower");
            auto belowUpper = builder.CreateICmpULE(cursorVal, upperVal, "below_upper");

            rangeTest = builder.CreateAnd(aboveLower, belowUpper, "is_between");
        }

        fitsARange[idx++] = rangeTest;
    }

    return builder.CreateOr(fitsARange);
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeGoToInst(const aotrc::compiler::InstructionPtr &inst) {
    auto goToInst = dynamic_cast<GoToInstruction *>(inst.get());

    // Get the destined block
    auto destBlock = static_cast<llvm::BasicBlock *>(this->symbolTable.at(getStateBlockLabel(goToInst->destId)));

    if (goToInst->testInstruction) {
        // If there is a test instruction, build it out, make a branch, and return the collect block
        auto canGoTo = this->makeInstruction(goToInst->testInstruction);

        // backup
        auto backupBlock = builder.GetInsertBlock();
        auto backupPoint = builder.GetInsertPoint();

        llvm::BasicBlock *onFalse, *collect;
        collect = llvm::BasicBlock::Create(ctx, "GO_TO_INST_COLLECT", function);

        onFalse = llvm::BasicBlock::Create(ctx, "GO_TO_INST_ON_FALSE", function, collect);
        builder.SetInsertPoint(onFalse);
        builder.CreateBr(collect);

        // Build a cond br
        builder.SetInsertPoint(backupBlock, backupPoint);
        builder.CreateCondBr(canGoTo, destBlock, onFalse);

        builder.SetInsertPoint(collect);
        return collect;

    } else {
        // If there is no condition, then just branch
        builder.CreateBr(destBlock);
        return nullptr;
    }
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeAcceptInst(const aotrc::compiler::InstructionPtr &inst) {
    auto acceptBlock = static_cast<llvm::BasicBlock *>(this->symbolTable.at("accept_block"));
    builder.CreateBr(acceptBlock);
    return nullptr;
}

llvm::Value * aotrc::compiler::FullMatchTranslator::makeRejectInst(const aotrc::compiler::InstructionPtr &inst) {
    auto rejectBlock = static_cast<llvm::BasicBlock *>(this->symbolTable.at("reject_block"));
    builder.CreateBr(rejectBlock);
    return nullptr;
}
