//
// Created by charlie on 12/5/21.
//

#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H

#include <string>
#include <vector>
#include <llvm/Support/CodeGen.h>

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

        llvm::CodeGenFileType getOutputType() const {
            return outputType;
        }

        const std::vector<std::string> &getInputFilePaths() const {
            return inputFilePaths;
        }

    private:
        int help;
        int version;
        llvm::CodeGenFileType outputType;
        std::vector<std::string> inputFilePaths;
    };
}

#endif //_ARGS_PARSER_H
