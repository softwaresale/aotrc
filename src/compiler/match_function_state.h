//
// Created by charlie on 12/5/21.
//

#ifndef _MATCH_FUNCTION_STATE_H
#define _MATCH_FUNCTION_STATE_H

#include <llvm/IR/BasicBlock.h>
#include "compiler_context.h"
#include "../fa/transition_table.h"

namespace aotrc::compiler {
    /**
     * Represents a complete state, which includes initial block, compute block, and edge blocks
     */
    class MatchFunctionState {
    public:
        MatchFunctionState();

        /**
         * Creates a new state, creates basic blocks and adds them all to the match function parent
         * @param ctx
         * @param matchFunction
         */
        MatchFunctionState(unsigned int state, bool isAccept, bool isLeaf, llvm::Function *parentFunc, const std::shared_ptr<CompilerContext> &ctx);

        void buildInitialState(llvm::Value *counterVar, llvm::Value *lengthArg, llvm::BasicBlock *accept, llvm::BasicBlock *reject);
        void buildComputeBlock(llvm::Value *counterVar, llvm::Value *inputTextPtr);
        void buildEdgeTo(llvm::Function *parent, const aotrc::compiler::MatchFunctionState &destState, const aotrc::fa::Edge &edge, llvm::BasicBlock *reject);
        void buildLinkToFirstEdge();

        bool leaf() const {
            return isLeaf;
        }

        llvm::BasicBlock *getInitialBlock() const {
            return initialBlock;
        }

    private:
        // State info
        std::string stateLabel;
        bool isAccept;
        bool isLeaf;
        // Compiler info
        std::shared_ptr<CompilerContext> ctx;
        llvm::BasicBlock *initialBlock;
        llvm::BasicBlock *computeBlock;
        std::vector<llvm::BasicBlock*> edgeBlocks;
        llvm::Value *counterVal;
        llvm::Value *cursorVal;
    };
}

#endif //_MATCH_FUNCTION_STATE_H
