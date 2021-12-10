//
// Created by charlie on 12/8/21.
//

#include "aotrc_input_parser.h"

aotrc::input::AotrcInputParser::AotrcInputParser(const std::string &path) {
    auto moduleDefs = YAML::LoadAllFromFile(path);

    // Read everything in
    for (const auto &node : moduleDefs) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            // Get the module label
            auto moduleLabel = it->first.as<std::string>();

            // Get all the patterns
            auto patternDefs = it->second;
            std::vector<RegexDef> regexDefs;
            for (auto patternsIt = patternDefs.begin(); patternsIt != patternDefs.end(); ++patternsIt) {
                auto label = patternsIt->first.as<std::string>();
                auto pattern = patternsIt->second.as<std::string>();

                regexDefs.emplace_back(label, pattern);
            }

            this->modules[moduleLabel] = std::move(regexDefs);
        }
    }
}
