//
// Created by charlie on 8/21/22.
//

#ifndef AOTRC_CAPTURE_TRANSLATOR_H
#define AOTRC_CAPTURE_TRANSLATOR_H

#include "src/compiler/full_match/full_match_translator.h"

namespace aotrc::compiler {
    class CaptureTranslator : public FullMatchTranslator {
    public:
        CaptureTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder, llvm::Function *func);

    public:
        llvm::Value *
        makeEntryBlock(std::vector<std::reference_wrapper<const InstructionPtr>> &&setupInstructions) override;

        llvm::Value *makeAcceptInst(const InstructionPtr &inst) override;

        //llvm::Value *makeStoreArrayInst(const InstructionPtr &inst) override;
    };
} // compiler

#endif //AOTRC_CAPTURE_TRANSLATOR_H
