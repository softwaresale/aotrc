//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_FULL_MATCH_TRANSLATOR_H
#define AOTRC_FULL_MATCH_TRANSLATOR_H

#include "src/compiler/instruction_translator.h"

namespace aotrc::compiler {
    /**
     * Specialized instruction translator for Full match DFAs
     */
    class FullMatchTranslator : public InstructionTranslator {
    public:
        FullMatchTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder, llvm::Function *func);

        /**
         * Execute a declare variable instruction. Creates a new variable and places it in the symbol
         * table.
         * @param inst declare var inst to execute
         * @return Pointer to alloca inst for the newly created variable
         */
        llvm::Value *makeDeclareVarInst(const InstructionPtr &inst) override;

        /**
         * Create a new basic block for the described state if one does not already exist. Also, sets the insertion
         * point on this particular block
         * @param inst StartStateInstruction inst
         * @return The basic block for the given state
         */
        llvm::Value *makeStartStateInst(const InstructionPtr &inst) override;

        /**
         * Gets the next character from the subject string, updates the counter variable
         * @param inst ConsumeInstruction inst
         * @return The value of character just loaded
         */
        llvm::Value *makeConsumeInst(const InstructionPtr &inst) override;

        /**
         * Checks if the end of the subject string has been reached. Sets the builder insertion point
         * to point on the collect block if nothing is to happen
         * @param inst CheckEndInstruction
         * @return The collect basic block generated by this function
         */
        llvm::Value *makeCheckEndInst(const InstructionPtr &inst) override;

        /**
         * Tests all of the ranges for a given edge and determines if it can be traversed
         * @param inst TestEdgeInstruction
         * @return Boolean value that determines if the test can be taken
         */
        llvm::Value *makeTestEdgeInst(const InstructionPtr &inst) override;

        /**
         * Creates a goto instruction. If the instruction is unconditional, then flow will branch to that
         * block, and null is returned. If the instruction is conditional, then execution will branch and
         * collect to a block, and the collect block will be returned
         * @param inst GoToInstruction to be compiled
         * @return nullptr if unconditional, collect block if conditional
         */
        llvm::Value *makeGoToInst(const InstructionPtr &inst) override;

        /**
         * Just branch to accept block
         * @param inst AcceptInstruction
         * @return nullptr
         */
        llvm::Value *makeAcceptInst(const InstructionPtr &inst) override;

        /**
         * Just branch to reject block
         * @param inst RejectInstruction
         * @return nullptr
         */
        llvm::Value *makeRejectInst(const InstructionPtr &inst) override;

        llvm::Value *makeStoreVarInst(const InstructionPtr &inst) override;
    };
}

#endif //AOTRC_FULL_MATCH_TRANSLATOR_H
