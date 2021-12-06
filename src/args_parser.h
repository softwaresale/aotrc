//
// Created by charlie on 12/5/21.
//

#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H

#include <string>
#include <vector>

namespace aotrc {
    class ArgsParser {
    public:
        ArgsParser(int argc, char **argv);

        void displayHelp() const;

        bool hasHelp() const {
            return this->help;
        }
        bool hasVersion() const {
            return this->version;
        }

        const std::vector<std::string> &getRemainingData() const {
            return remainingData;
        }

    private:
        int help;
        int version;
        std::vector<std::string> remainingData;
    };
}

#endif //_ARGS_PARSER_H
