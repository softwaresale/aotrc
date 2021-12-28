//
// Created by charlie on 12/6/21.
//

#include "code_gen.h"

#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/IR/LegacyPassManager.h>
#include <fstream>
#include <sstream>
#include <utility>

#include "compiler_context.h"

aotrc::compiler::CodeGen::CodeGen(std::shared_ptr<aotrc::compiler::CompilerContext> ctx)
: ctx(std::move(ctx)) {
    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(LLVM_DEFAULT_TARGET_TRIPLE, error);
    if (!target) {
        throw std::runtime_error(error);
    }

    llvm::TargetOptions options;
    this->machine = std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(LLVM_DEFAULT_TARGET_TRIPLE, "generic", "", options, llvm::Optional<llvm::Reloc::Model>()));
}

bool aotrc::compiler::CodeGen::compileModule(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath, llvm::CodeGenFileType outputType) {
    // Set up some basics of the module
    module->setDataLayout(this->machine->createDataLayout());
    module->setCodeModel(this->machine->getCodeModel());

    // Open the file to emit to
    std::error_code err;
    llvm::raw_fd_ostream outputStream(outputPath, err);
    if (err) {
        llvm::errs() << "Could not open file: " << err.message();
        return false;
    }

    // Add a pass to generate machine code
    llvm::legacy::PassManager passManager;
    if (this->machine->addPassesToEmitFile(passManager, outputStream, nullptr, outputType)) {
        llvm::errs() << "Error while emitting to file";
        return false;
    }

    // Run all the passes
    passManager.run(*module);

    // Flush the stream and such
    outputStream.flush();

    return true;
}

static std::string llvmTypeToCType(llvm::Type *type, const std::shared_ptr<aotrc::compiler::CompilerContext> &ctx) {
    static auto boolWidth = llvm::IntegerType::getInt1Ty(ctx->context())->getBitWidth();
    static auto charWidth = llvm::IntegerType::getInt8Ty(ctx->context())->getBitWidth();
    static auto shortWidth = llvm::IntegerType::getInt16Ty(ctx->context())->getBitWidth();
    static auto intWidth = llvm::IntegerType::getInt32Ty(ctx->context())->getBitWidth();
    static auto longWidth = llvm::IntegerType::getInt64Ty(ctx->context())->getBitWidth();
    std::stringstream typeStr;

    switch (type->getTypeID()) {

        case llvm::Type::PointerTyID: {
            auto ptr = (llvm::PointerType *) type;
            std::string elementType = llvmTypeToCType(ptr->getElementType(), ctx);
            typeStr << elementType << "*";
            return typeStr.str();
        }

        case llvm::Type::IntegerTyID: {
            auto intType = (llvm::IntegerType *) type;
            if (intType->getBitWidth() == boolWidth) {
                typeStr << "bool";
            } else if (intType->getBitWidth() == charWidth) {
                typeStr << "char";
            } else if (intType->getBitWidth() == shortWidth) {
                typeStr << "short";
            } else if (intType->getBitWidth() == intWidth) {
                typeStr << "int";
            } else if (intType->getBitWidth() == longWidth) {
                typeStr << "long";
            }

            break;
        }

        default:
            throw std::runtime_error("Invalid type");
    }

    return typeStr.str();
}

static std::string createDefForFunction(const llvm::Function *func, const std::shared_ptr<aotrc::compiler::CompilerContext> &ctx) {
    std::stringstream defString;
    // Write the return type
    auto returnType = func->getReturnType();
    defString << llvmTypeToCType(returnType, ctx);

    // Add a space
    defString << ' ';

    // Write the name
    defString << func->getName().str() << '(';

    // If there are no args, then we're done here
    if (func->arg_size() == 0) {
        defString << ");";
        return defString.str();
    }

    // Write the args
    auto argIt = func->arg_begin();
    for (; argIt != func->arg_end() - 1; ++argIt) {
        // get the type and name (?) of the argument
        auto argType = argIt->getType();
        auto argName = argIt->getName().str();

        defString << llvmTypeToCType(argType, ctx);
        if (!argName.empty()) {
            defString << ' ' << argName;
        }

        defString << ", ";
    }
    // do the last arg without a comma
    auto argType = argIt->getType();
    auto argName = argIt->getName().str();

    defString << llvmTypeToCType(argType, ctx);
    if (!argName.empty()) {
        defString << ' ' << argName;
    }

    defString << ");";

    return defString.str();
}

void
aotrc::compiler::CodeGen::generateHeader(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath) {
    std::ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Could not open generated output file");
    }

    // Create the starting header guard
    std::string headerGuardVariable = "_AOTRC_MODULE_" + module->getName().str() + "_H";
    outputFile << "#ifndef " << headerGuardVariable << std::endl;
    outputFile << "#define " << headerGuardVariable << std::endl;
    outputFile << std::endl;
    outputFile << "// This file is auto generated by AOTRC" << std::endl << std::endl;
    outputFile << "#include <stdbool.h>" << std::endl;
    // Write function defs
    for (const auto &function : module->functions()) {
        outputFile << createDefForFunction(&function, this->ctx) << std::endl;
    }

    // Write the closing header guard
    outputFile << "#endif //" << headerGuardVariable << std::endl;
}
