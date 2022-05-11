//
// Created by charlie on 5/11/22.
//

#include "sub_match_program_info_provider.h"

std::string
aotrc::compiler::SubMatchProgramInfoProvider::getFunctionName(const std::string &regexLabel) const noexcept {
    return regexLabel + "_sub_match";
}
