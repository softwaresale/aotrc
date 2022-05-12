//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_INSTRUCTION_TRANSLATOR_H
#define AOTRC_INSTRUCTION_TRANSLATOR_H

#include <string>
#include <unordered_map>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include "program.h"

namespace aotrc::compiler {

    /**
     * Class that translates high-level instructions into LLVM. This visitor pattern takes in HIR and
     * translates it to corresponding LLVM IR. This class can be extended if each instruction should be
     * translated differently depending on the type of class.
     */
    class InstructionTranslator {
    public:
        /**
         * Creates a new instruction translator
         * @param ctx The LLVM context being used
         * @param builder The LLVM IR builder to use in building the function
         * @param func The function that we are compiling this program into
         */
        InstructionTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder, llvm::Function *func);

        /**
         * Compiles any kind of instruction. This is a wrapper around specific makeInstruction functions
         * defined in this class
         * @param inst Any instruction
         * @return Dependent on instruction type
         */
        llvm::Value *makeInstruction(const InstructionPtr &inst);

        /**
         * Builds out the entry block of a function. This entry block is responsible for compiling a list
         * of setup instructions and placing them into the entry block, which is the first basic block in
         * the function. Places the entry block into the symbol table.
         * @param setupInstructions A list of instructions to compile
         * @return The entry block created.
         */
        virtual llvm::Value *makeEntryBlock(std::vector<std::reference_wrapper<const InstructionPtr>> &&setupInstructions);

        std::pair<llvm::BasicBlock *, llvm::BasicBlock::iterator> storeInsertPoint();
        void restoreInsertPoint(std::pair<llvm::BasicBlock *, llvm::BasicBlock::iterator> state);

        /**
         * Builds the accept block used by this program. Places the block in the symbol table. Note: should not
         * move the insertion point.
         * @return A pointer to the accept block
         */
        virtual llvm::Value *makeAcceptBlock();

        /**
         * Builds the reject block used by this program. Places the block in the symbol table. Note: should not
         * move the insertion point.
         * @return A pointer to the reject block
         */
        virtual llvm::Value *makeRejectBlock();

        /**
         * Builds the link between the entry block and the first state
         * @return null
         */
        virtual llvm::Value *linkFirstStateToEntry();

        virtual llvm::Value *makeDeclareVarInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeStartStateInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeConsumeInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeCheckEndInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeTestEdgeInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeGoToInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeAcceptInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeRejectInst(const InstructionPtr &inst) = 0;
        virtual llvm::Value *makeStoreVarInst(const InstructionPtr &inst) = 0;

    protected:
        llvm::LLVMContext &ctx;
        llvm::IRBuilder<> &builder;
        llvm::Function *function;
        /// Defines specific symbols used in the program (e.g. variables, blocks, etc)
        std::unordered_map<std::string, llvm::Value *> symbolTable;
    };

}

#endif //AOTRC_INSTRUCTION_TRANSLATOR_H
