//
// Created by charlie on 12/8/21.
//

#ifndef _AOTRC_INPUT_PARSER_H
#define _AOTRC_INPUT_PARSER_H

#include <yaml-cpp/yaml.h>

namespace aotrc::input {

    struct RegexDef {
    public:
        RegexDef(const std::string &label, const std::string &pattern)
        : label(label) {
            // Remove the //'s from pattern
            this->pattern = pattern.substr(1, pattern.size() - 2);
        }

        std::string label;
        std::string pattern;
    };

    class AotrcInputParser {
    public:
        explicit AotrcInputParser(const std::string &path);

        const std::unordered_map<std::string, std::vector<RegexDef>> &getModules() const {
            return modules;
        }

    private:
        std::unordered_map<std::string, std::vector<RegexDef>> modules;
    };
}

#endif //_AOTRC_INPUT_PARSER_H
