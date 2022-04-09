//
// Created by charlie on 3/14/22.
//

#ifndef ATORC_PROGRAM_STATE_H
#define ATORC_PROGRAM_STATE_H

#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <optional>

namespace aotrc::compiler {
    /**
     * This class wraps all of the state used in a regular expression program. This state
     * gets passed around to various build methods and such so it is not a part of the
     * actual program class.
     */
    class ProgramState {
    public:
        /**
         * Creates a new program state function out of the function that the program
         * is going to get compiled into
         * @param programFunc Function that the program is going to be compiled into
         */
        explicit ProgramState(llvm::Function *programFunc);

        llvm::LLVMContext &ctx() {
            return context;
        }

        llvm::IRBuilder<> &builder() {
            return irBuilder;
        }

        llvm::Function *getParentFunction() const {
            return parentFunction;
        }

        llvm::Value *getSubject() const {
            return subject;
        }

        llvm::Value *getSubjectLength() const {
            return subjectLength;
        }

        llvm::AllocaInst *getCounter() const {
            return counter;
        }

        llvm::AllocaInst *getCursor() const {
            return cursor;
        }

        const std::unordered_map<unsigned int, llvm::BasicBlock *> &getStateBlocks() const {
            return stateBlocks;
        }

        llvm::BasicBlock *getAcceptBlock() const {
            return acceptBlock;
        }

        llvm::BasicBlock *getRejectBlock() const {
            return rejectBlock;
        }

        /**
         * Gets the state block for state or creates a new one
         * @param state state id to create
         * @return      basic block for the provided state
         */
        llvm::BasicBlock *stateBlock(unsigned int state);

        /**
         * Gets a state block if it exists
         * @param state State to retrieve
         * @return      Optional block for the state
         */
        std::optional<llvm::BasicBlock *> stateBlockAt(unsigned int state) const;

    private:
        llvm::LLVMContext &context;
        llvm::IRBuilder<> irBuilder;
        llvm::Function *parentFunction;
        llvm::Value *subject;
        llvm::Value *subjectLength;
        llvm::AllocaInst *counter;
        llvm::AllocaInst *cursor;
        std::unordered_map<unsigned int, llvm::BasicBlock*> stateBlocks;
        llvm::BasicBlock *acceptBlock;
        llvm::BasicBlock *rejectBlock;
    };

    /**
     * This is a program state extension for a program that is doing sub-matching. This
     * state provides an extra variable that tracks is an accept state has been entered.
     */
    class SubMatchProgramState : public ProgramState {
    public:
        explicit SubMatchProgramState(llvm::Function *programFunc);

        llvm::AllocaInst *getMatchEncountered() const {
            return matchEncountered;
        }

    private:
        llvm::AllocaInst *matchEncountered;
    };
}

#endif //ATORC_PROGRAM_STATE_H
