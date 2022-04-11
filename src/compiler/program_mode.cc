//
// Created by charlie on 3/19/22.
//

#include "program_mode.h"

static std::unordered_map<aotrc::compiler::ProgramMode, std::unique_ptr<aotrc::compiler::BaseProgramMode>>
initProgramModeMap() {
    std::unordered_map<aotrc::compiler::ProgramMode, std::unique_ptr<aotrc::compiler::BaseProgramMode>> typesMap;
    typesMap[aotrc::compiler::ProgramMode::FULL_MATCH] = std::make_unique<aotrc::compiler::FullMatchProgramMode>();
    typesMap[aotrc::compiler::ProgramMode::SUB_MATCH] = std::make_unique<aotrc::compiler::SubMatchProgramMode>();
    typesMap[aotrc::compiler::ProgramMode::SEARCH] = std::make_unique<aotrc::compiler::SearchProgramMode>();

    return typesMap;
}

const std::unique_ptr<aotrc::compiler::BaseProgramMode> &
aotrc::compiler::getProgramType(aotrc::compiler::ProgramMode type) {
    static std::unordered_map<ProgramMode, std::unique_ptr<BaseProgramMode>> typesMap = initProgramModeMap();

    return typesMap.at(type);
}
