//
// Created by charlie on 12/19/21.
//

#include "build_pattern_func.h"

static llvm::GlobalVariable *
build_global(const std::string &label, const std::string &pattern,
             std::shared_ptr<llvm::Module> &parent, const std::shared_ptr<aotrc::compiler::CompilerContext> &ctx) {

    // Create a global character array that holds the pattern
    std::string uppercaseLabel = label;
    std::transform(uppercaseLabel.begin(),  uppercaseLabel.end(), uppercaseLabel.begin(), ::toupper);
    std::string globalVarName = uppercaseLabel + "_PATTERN";
    auto charArrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx->context()), pattern.length() + 1);
    parent->getOrInsertGlobal(globalVarName, charArrayType);
    auto patternGlobal = parent->getGlobalVariable(globalVarName);

    // Create a constant array out of the pattern
    std::vector<llvm::Constant *> patternCharacterConstants;
    auto charType = llvm::Type::getInt8Ty(ctx->context());
    for (const auto &c : pattern) {
        auto literalChar = llvm::ConstantInt::get(charType, (long) c);
        patternCharacterConstants.push_back(literalChar);
    }
    // Add a null terminator
    patternCharacterConstants.push_back(llvm::ConstantInt::get(charType, (long) 0));

    // Set a constant initializer
    patternGlobal->setInitializer(llvm::ConstantArray::get(charArrayType, patternCharacterConstants));

    return patternGlobal;
}

bool aotrc::compiler::build_pattern_func(const std::string &label, const std::string &pattern,
                        std::shared_ptr<llvm::Module> &parent, const std::shared_ptr<aotrc::compiler::CompilerContext> &ctx) {

    // Build a global that holds the array
    auto patternGlobal = build_global(label, pattern, parent, ctx);

    // Build a function that returns said array
    auto charPtrType = llvm::Type::getInt8Ty(ctx->context())->getPointerTo();
    auto getPatternFunctionType = llvm::FunctionType::get(charPtrType, false);
    std::string functionName = "get_pattern_" + label;
    parent->getOrInsertFunction(functionName, getPatternFunctionType);
    auto getFunc = parent->getFunction(functionName);

    // Return a pointer to the global?
    auto mainBB = llvm::BasicBlock::Create(ctx->context(), "main", getFunc);
    ctx->builder().SetInsertPoint(mainBB);

    // Get pointer to the global
    auto ptr = ctx->builder().CreateGEP(charPtrType, patternGlobal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx->context()), 0), "pointerToFirst");
    ctx->builder().CreateRet(ptr);

    return true;
}
