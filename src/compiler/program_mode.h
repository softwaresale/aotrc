//
// Created by charlie on 3/19/22.
//

#ifndef AOTRC_PROGRAM_MODE_H
#define AOTRC_PROGRAM_MODE_H

#include <string>
#include <unordered_map>
#include <llvm/IR/DerivedTypes.h>
#include "program_state.h"

namespace aotrc::compiler {

    enum ProgramMode {
        FULL_MATCH,
        SUB_MATCH,
    };

    struct BaseProgramMode {
        virtual std::string getFunctionName(const std::string &label) const = 0;
        virtual llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const = 0;
        virtual std::unique_ptr<aotrc::compiler::ProgramState> getProgramState(llvm::Function *parentFunc) = 0;
    };

    struct FullMatchProgramMode : public BaseProgramMode {
        std::string getFunctionName(const std::string &label) const override {
            return label + "_full_match";
        }

        llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const override {
            auto boolType = llvm::Type::getInt1Ty(ctx);
            auto charPtrType = llvm::Type::getInt8PtrTy(ctx);
            auto sizeType = llvm::Type::getInt64Ty(ctx);
            return llvm::FunctionType::get(boolType, { charPtrType, sizeType }, false);
        }

        std::unique_ptr<ProgramState> getProgramState(llvm::Function *parentFunc) override {
            return std::make_unique<ProgramState>(parentFunc);
        }
    };

    struct SubMatchProgramMode : public BaseProgramMode {
        std::string getFunctionName(const std::string &label) const override {
            return label + "_sub_match";
        }

        llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const override {
            auto boolType = llvm::Type::getInt1Ty(ctx);
            auto charPtrType = llvm::Type::getInt8PtrTy(ctx);
            auto sizeType = llvm::Type::getInt64Ty(ctx);
            return llvm::FunctionType::get(boolType, { charPtrType, sizeType }, false);
        }

        std::unique_ptr<ProgramState> getProgramState(llvm::Function *parentFunc) override {
            return std::make_unique<SubMatchProgramState>(parentFunc);
        }
    };

    /* TODO:
     * it would be nice to try to move away from this pattern and instead
     * favor something like template metaprogramming. This pattern uses static storage
     * which seems like a bad use of system resources. However, the static storage isn't very much
     * so it'll slide for now
     */
    const std::unique_ptr<BaseProgramMode> &getProgramType(ProgramMode type);
}

#endif //AOTRC_PROGRAM_MODE_H
