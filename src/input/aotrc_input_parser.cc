//
// Created by charlie on 12/8/21.
//

#include "aotrc_input_parser.h"

static aotrc::input::RegexDef regexDefFromObject(const std::string &label, const YAML::Node &mapNode) {
    auto pattern = mapNode["pattern"].as<std::string>();
    auto options = mapNode["options"];
    bool genFullMatch = true;
    if (options["fullmatch"]) {
        genFullMatch = options["fullmatch"].as<bool>();
    }

    bool genSubMatch = false;
    if (options["submatch"]) {
        genSubMatch = options["submatch"].as<bool>();
    }

    return {label, pattern, genFullMatch, genSubMatch};
}

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

                // If the thing is just a string, then it's simple
                if (patternsIt->second.IsScalar()) {
                    auto pattern = patternsIt->second.as<std::string>();
                    regexDefs.emplace_back(label, pattern);
                } else if (patternsIt->second.IsMap()) {
                    // Gotta break down the object into a regex def
                    auto def = regexDefFromObject(label, patternsIt->second);
                    regexDefs.push_back(def);
                }
            }

            this->modules[moduleLabel] = std::move(regexDefs);
        }
    }
}
