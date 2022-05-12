//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_SUB_MATCH_PROGRAM_INFO_PROVIDER_H
#define AOTRC_SUB_MATCH_PROGRAM_INFO_PROVIDER_H

#include "src/compiler/full_match/full_match_program_info_provider.h"

namespace aotrc::compiler {
    struct SubMatchProgramInfoProvider : public FullMatchProgramInfoProvider {
        std::string getFunctionName(const std::string &regexLabel) const noexcept override;
    };
}

#endif //AOTRC_SUB_MATCH_PROGRAM_INFO_PROVIDER_H
