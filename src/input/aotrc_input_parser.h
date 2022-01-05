//
// Created by charlie on 12/8/21.
//

#ifndef _AOTRC_INPUT_PARSER_H
#define _AOTRC_INPUT_PARSER_H

#include <yaml-cpp/yaml.h>

namespace aotrc::input {

    /**
     * Represents a regex def. Contains the regex label, regex pattern, and any options associated with it
     */
    struct RegexDef {
    public:
        RegexDef(const std::string &label, const std::string &pattern, bool fullmatch = true, bool submatch = false)
        : label(label)
        , genFullMatch(fullmatch)
        , genSubMatch(submatch) {
            // Remove the //'s from pattern
            this->pattern = pattern.substr(1, pattern.size() - 2);
        }

        std::string label;
        std::string pattern;
        bool genFullMatch;
        bool genSubMatch;
    };

    /**
     * Parses aotrc input files for module definitions
     */
    class AotrcInputParser {
    public:
        /**
         * Parse a regex file
         * @param path Path to the input file
         */
        explicit AotrcInputParser(const std::string &path);

        const std::unordered_map<std::string, std::vector<RegexDef>> &getModules() const {
            return modules;
        }

    private:
        std::unordered_map<std::string, std::vector<RegexDef>> modules;
    };
}

#endif //_AOTRC_INPUT_PARSER_H
