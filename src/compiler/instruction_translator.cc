//
// Created by charlie on 5/11/22.
//

#include "instruction_translator.h"

aotrc::compiler::InstructionTranslator::InstructionTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder,
                                                              llvm::Function *func)
                                                              : ctx(ctx)
                                                              , builder(builder)
                                                              , function(func) {
}

llvm::Value * aotrc::compiler::InstructionTranslator::makeInstruction(const aotrc::compiler::InstructionPtr &inst) {
    llvm::Value *val;
    switch (inst->type()) {
        case START_STATE:
            val = this->makeStartStateInst(inst);
            break;
        case CONSUME:
            val = this->makeConsumeInst(inst);
            break;
        case CHECK_END:
            val = this->makeCheckEndInst(inst);
            break;
        case DECLARE_VAR:
            val = this->makeDeclareVarInst(inst);
            break;
        case TEST:
            val = this->makeTestEdgeInst(inst);
            break;
        case GOTO:
            val = this->makeGoToInst(inst);
            break;
        case ACCEPT:
            val = this->makeAcceptInst(inst);
            break;
        case REJECT:
            val = this->makeRejectInst(inst);
            break;
        case STORE_VAR:
            val = this->makeStoreVarInst(inst);
            break;
        default:
            throw std::runtime_error("InstructionTranslator - invalid instruction");
    }

    return val;
}

llvm::Value *
aotrc::compiler::InstructionTranslator::makeEntryBlock(std::vector<std::reference_wrapper<const InstructionPtr>> &&setupInstructions) {
    // Make the entry block
    auto entryBlock = llvm::BasicBlock::Create(ctx, "ENTRY", this->function);
    builder.SetInsertPoint(entryBlock);
    // Build all setup instructions in the entry block
    for (const auto &inst : setupInstructions) {
        this->makeInstruction(inst);
    }
    // Add the setup block to the symbol table
    this->symbolTable["entry_block"] = entryBlock;

    return entryBlock;
}

llvm::Value *aotrc::compiler::InstructionTranslator::makeAcceptBlock() {
    // Backup location
    auto backupBlock = builder.GetInsertBlock();
    auto backupPoint = builder.GetInsertPoint();
    // Make the accept block, return true
    auto acceptBlock = llvm::BasicBlock::Create(ctx, "ACCEPT", function);
    builder.SetInsertPoint(acceptBlock);
    builder.CreateRet(llvm::ConstantInt::getTrue(ctx));
    // Restore location
    builder.SetInsertPoint(backupBlock, backupPoint);
    // Place the accept block in symbol table
    this->symbolTable["accept_block"] = acceptBlock;
    // return the block
    return acceptBlock;
}

llvm::Value *aotrc::compiler::InstructionTranslator::makeRejectBlock() {
    // Backup location
    auto backupBlock = builder.GetInsertBlock();
    auto backupPoint = builder.GetInsertPoint();
    // Make the reject block, return true
    auto rejectBlock = llvm::BasicBlock::Create(ctx, "REJECT", function);
    builder.SetInsertPoint(rejectBlock);
    builder.CreateRet(llvm::ConstantInt::getFalse(ctx));
    // Restore location
    builder.SetInsertPoint(backupBlock, backupPoint);
    // Place the accept block in symbol table
    this->symbolTable["reject_block"] = rejectBlock;
    // return the block
    return rejectBlock;
}

std::pair<llvm::BasicBlock *, llvm::BasicBlock::iterator> aotrc::compiler::InstructionTranslator::storeInsertPoint() {
    return { this->builder.GetInsertBlock(), this->builder.GetInsertPoint() };
}

void aotrc::compiler::InstructionTranslator::restoreInsertPoint(std::pair<llvm::BasicBlock *, llvm::BasicBlock::iterator> state) {
    auto [block, pos] = state;
    this->builder.SetInsertPoint(block, pos);
}

llvm::Value *aotrc::compiler::InstructionTranslator::linkFirstStateToEntry() {
    auto entryBlock = static_cast<llvm::BasicBlock *>(this->symbolTable.at("entry_block"));
    auto firstStateBlock = static_cast<llvm::BasicBlock *>(this->symbolTable.at(getStateBlockLabel(0)));
    this->builder.SetInsertPoint(entryBlock);
    this->builder.CreateBr(firstStateBlock);
    return nullptr;
}
