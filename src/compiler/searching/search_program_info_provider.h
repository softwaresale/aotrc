//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_SEARCH_PROGRAM_INFO_PROVIDER_H
#define AOTRC_SEARCH_PROGRAM_INFO_PROVIDER_H

#include "src/compiler/program_info_provider.h"

namespace aotrc::compiler {
    struct SearchProgramInfoProvider : public ProgramInfoProvider {
        llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const override;

        std::string getFunctionName(const std::string &regexLabel) const noexcept override;
    };
}

#endif //AOTRC_SEARCH_PROGRAM_INFO_PROVIDER_H
