//
// Created by charlie on 12/5/21.
//

#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H

#include <string>
#include <vector>
#include <variant>
#include <llvm/Support/CodeGen.h>

namespace aotrc {

    enum OutputType {
        ASM,
        OBJ,
        IR,
    };

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

        bool hasCodeGenType() {
            return this->outputType == OBJ || this->outputType == ASM;
        }

        llvm::CodeGenFileType outputTypeAsCodeGen() const {
            switch(this->outputType) {
                case OBJ:
                    return llvm::CodeGenFileType::CGFT_ObjectFile;
                case ASM:
                    return llvm::CodeGenFileType::CGFT_AssemblyFile;
                default:
                    return llvm::CodeGenFileType::CGFT_Null;
            }
        }

        OutputType getOutputType() const {
            return outputType;
        }

        const std::vector<std::string> &getInputFilePaths() const {
            return inputFilePaths;
        }

    private:
        int help;
        int version;
        OutputType outputType;
        std::vector<std::string> inputFilePaths;
    };
}

#endif //_ARGS_PARSER_H
