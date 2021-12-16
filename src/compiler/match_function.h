//
// Created by charlie on 12/4/21.
//

#ifndef _MATCH_FUNCTION_H
#define _MATCH_FUNCTION_H

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include "compiler_context.h"
#include "../fa/dfa.h"
#include "match_function_state.h"

namespace aotrc::compiler {

    /**
     * Represents a function that matches a regex. It is responsible for holding some relevant
     * state info
     */
    class MatchFunction {
    public:
        /**
         * Creates a new match function
         * @param label label of the regex to match
         * @param parentModule module to add this function to
         * @param ctx compiler ctx singleton
         */
        MatchFunction(aotrc::fa::DFA&& dfa, const std::string &label, bool isSubMatch, std::shared_ptr<llvm::Module> &parentModule, const std::shared_ptr<CompilerContext>& ctx);

        llvm::Value *getPatternLengthArg();
        llvm::Value *getPatternArg();
        void build();

    private:

        // DFA to build out
        aotrc::fa::DFA dfa;
        // Pointer to the compiler ctx singleton. Holds llvm ctx, ir builder, and module repository
        std::shared_ptr<CompilerContext> ctx;
        // Pointer to the actual function type
        llvm::Function *matchFunction;
        // tracks where we are in the input string
        llvm::AllocaInst *counterVar;
        // Block where we set up the basic stuff
        llvm::BasicBlock *initialBlock;
        // Accept and reject blocks are the final stage
        llvm::BasicBlock *acceptBlock;
        llvm::BasicBlock *rejectBlock;
        // Collection of state objects
        std::unordered_map<unsigned int, aotrc::compiler::MatchFunctionState> states;
        bool isSubMatch;
    };
}

#endif //_MATCH_FUNCTION_H
