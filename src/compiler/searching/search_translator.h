//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_SEARCH_TRANSLATOR_H
#define AOTRC_SEARCH_TRANSLATOR_H

#include "src/compiler/full_match/full_match_translator.h"

namespace aotrc::compiler {

    class SearchTranslator : public FullMatchTranslator {
    public:
        SearchTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder, llvm::Function *func);

        /**
         * When accepting, we also need to actually store the positions into the parameters. So, we first
         * do that, then we accept.
         * @param inst
         * @return nullptr
         */
        llvm::Value *makeAcceptInst(const InstructionPtr &inst) override;

        llvm::Value *makeGoToInst(const InstructionPtr &inst) override;

        /**
         * This instruction is actually used here. It stores the value described by the instruction
         * into the destination variable
         * @param inst
         * @return nullptr
         */
        llvm::Value *makeStoreVarInst(const InstructionPtr &inst) override;

    private:
        // TODO check store variable block function that makes a block that stores a variable into another one
        llvm::BasicBlock *
        createStoreBlock(llvm::AllocaInst *valueToStore, llvm::Value *dest, llvm::BasicBlock *collectBlock);

        llvm::Value *buildNonNullCheck(llvm::Value *valueToCheck);

        llvm::BasicBlock *
        createNonNullStore(llvm::AllocaInst *varToStore, llvm::Value *potentialDest);
    };
}

#endif //AOTRC_SEARCH_TRANSLATOR_H
