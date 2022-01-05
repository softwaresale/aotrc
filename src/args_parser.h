//
// Created by charlie on 12/5/21.
//

#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H

#include <string>
#include <vector>
#include <variant>
#include <unordered_set>
#include <llvm/Support/CodeGen.h>

namespace aotrc {

    enum OutputType {
        ASM,
        OBJ,
        IR,
    };

    enum GraphVizOutputTypes {
        NFA,
        DFA_FULL,
        DFA_SUB
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

        const std::string &getLinkerPath() const {
            return linkerPath;
        }

        const std::string &getArPath() const {
            return arPath;
        }

        bool skipBuildShared() const {
            return this->skipShared;
        }

        bool skipBuildArchive() const {
            return this->skipArchive;
        }

        const std::unordered_set<GraphVizOutputTypes> &getGraphvizTypes() const {
            return graphvizTypes;
        }

    private:
        int help;
        int version;
        OutputType outputType;
        std::vector<std::string> inputFilePaths;
        std::string linkerPath;
        int skipShared;
        std::string arPath;
        int skipArchive;
        std::unordered_set<GraphVizOutputTypes> graphvizTypes;
    };
}

#endif //_ARGS_PARSER_H
