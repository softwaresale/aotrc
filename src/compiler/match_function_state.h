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
     * Represents a complete state, which includes initial block, compute block, and edge blocks. The initial block
     * is responsible for determining checking if we are at the end of the input string and if we need to accept or
     * reject. The compute block loads the next character to test against. Edge blocks encode each outgoing edge of the
     * DFA state. They check if the current character can take the edge. If it can, jumps to the destination state.
     */
    class MatchFunctionState {
    public:
        explicit MatchFunctionState(bool submatch = false);

        /**
         * Creates a new match function state
         * @param state The DFA state represented by this match function state
         * @param isAccept if this state is an accept state
         * @param isLeaf if this state is a leaf (has no outgoing edges)
         * @param parentFunc The match function this state belongs to
         * @param ctx compiler context
         * @param isSubMatch if we are submatching
         */
        MatchFunctionState(unsigned int state, bool isAccept, bool isLeaf, llvm::Function *parentFunc, const std::shared_ptr<CompilerContext> &ctx, bool isSubMatch = false);

        /**
         * Builds the LLVM IR for the initial state.
         * @param counterVar Variable that keeps track of where we are in the input string
         * @param lengthArg Argument that tells how long the input string is
         * @param accept Pointer to the basic block for rejecting
         * @param reject Pointer to the basic block for rejecting
         */
        void buildInitialState(llvm::Value *counterVar, llvm::Value *lengthArg, llvm::BasicBlock *accept, llvm::BasicBlock *reject);

        /**
         * Builds the LLVM IR for the compute block.
         * @param counterVar Variable that keeps track of where we are in the input string
         * @param inputTextPtr Pointer to the input text
         */
        void buildComputeBlock(llvm::Value *counterVar, llvm::Value *inputTextPtr);

        /**
         * Builds an edge to a given state
         * @param parent Function that this match state is a part of
         * @param destState State to transition to
         * @param edge The edge that leads to the destination state
         * @param reject The block to go to if we want to reject
         */
        void buildEdgeTo(llvm::Function *parent, const aotrc::compiler::MatchFunctionState &destState, const aotrc::fa::Edge &edge, llvm::BasicBlock *reject);

        /**
         * Builds a link from the compute block to the first edge block
         */
        void buildLinkToFirstEdge();

        inline bool leaf() const {
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

        bool isSubMatch;
    };
}

#endif //_MATCH_FUNCTION_STATE_H
