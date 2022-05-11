//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_FULL_MATCH_PROGRAM_INFO_PROVIDER_H
#define AOTRC_FULL_MATCH_PROGRAM_INFO_PROVIDER_H

#include "../program_info_provider.h"

namespace aotrc::compiler {
    struct FullMatchProgramInfoProvider : public ProgramInfoProvider {
        llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const override;

        std::string getFunctionName(const std::string &regexLabel) const noexcept override;
    };
}

#endif //AOTRC_FULL_MATCH_PROGRAM_INFO_PROVIDER_H
