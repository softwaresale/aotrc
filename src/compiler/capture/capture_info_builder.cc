//
// Created by charlie on 8/21/22.
//

#include <sstream>
#include "capture_info_builder.h"
#include "src/compiler/util.h"

aotrc::compiler::CaptureInfoBuilder::CaptureInfoBuilder(llvm::LLVMContext &ctx, const std::string &regexName,
                                                        std::vector<std::string> groupNames)
                                                        : ctx(ctx)
                                                        , groupNames(std::move(groupNames)) {

    // Set the full structure name, which is the
    std::stringstream nameFormat;
    nameFormat << regexName << "CaptureInfo";
    this->name = nameFormat.str();

    // Create necessary types for the body to use
    // Each member is a two element array. The first element is the start, the second is the end
    auto sizeType = llvm::Type::getInt64Ty(ctx);
    for (unsigned int i = 0; i < this->groupNames.size(); i++) {
        auto captureGroupArray = llvm::ArrayType::get(sizeType, 2);
        this->groupTypes.push_back(captureGroupArray);
    }
}

void aotrc::compiler::CaptureInfoBuilder::writeCStructTemplate(std::ostream &os) const {
    os << "struct " << this->name << " {\n";
    for (unsigned i = 0; i < this->groupNames.size(); i++) {
        os << '\t' << llvmTypeToCType(this->ctx, this->groupTypes[i]->getArrayElementType()) << ' ' << groupNames[i] << '[' << this->groupTypes[i]->getArrayNumElements() << "];\n";
    }
    os << '}';
}

llvm::StructType *aotrc::compiler::CaptureInfoBuilder::buildStruct() const {
    return llvm::StructType::create(ctx, this->groupTypes, this->name);
}
