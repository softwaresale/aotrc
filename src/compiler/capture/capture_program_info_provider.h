//
// Created by charlie on 8/21/22.
//

#ifndef AOTRC_CAPTURE_PROGRAM_INFO_PROVIDER_H
#define AOTRC_CAPTURE_PROGRAM_INFO_PROVIDER_H

#include "src/compiler/program_info_provider.h"

namespace aotrc::compiler {
    struct CaptureProgramInfoProvider : public ProgramInfoProvider {
    public:
        explicit CaptureProgramInfoProvider(llvm::StructType *captureInfoStructTp)
        : ProgramInfoProvider()
        , captureInfoStruct(captureInfoStructTp)
        {}

        llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const override;

        std::string getFunctionName(const std::string &regexLabel) const noexcept override;

    private:
        llvm::StructType *captureInfoStruct;
    };
}

#endif //AOTRC_CAPTURE_PROGRAM_INFO_PROVIDER_H
