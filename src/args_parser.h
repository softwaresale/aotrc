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
        STATIC,
        SHARED
    };

    enum GraphVizOutputTypes {
        NFA,
        DFA_FULL,
        DFA_SUB
    };

    /**
     * Class that abstracts away passing command line arguments. This class parses command line arguments and contains
     * and internal state of which options are set
     */
    class ArgsParser {
    public:
        /**
         * Parse command line arguments
         * @param argc number of arguments
         * @param argv list of arguments
         */
        ArgsParser(int argc, char **argv);

        /**
         * Prints help text
         */
        static void displayHelp();

        /**
         * Prints the version text
         */
        static void displayVersion();

        /**
         * Returns if help flag is set
         * @return t/f if set
         */
        bool hasHelp() const {
            return this->help;
        }

        /**
         * Returns if the version flag is set
         * @return t/f if version is set
         */
        bool hasVersion() const {
            return this->version;
        }

        /**
         * True if the compiler should generate a object code or assembly code
         * @return True if output type is obj or asm
         */
        bool hasCodeGenType() {
            return this->outputType == OBJ || this->outputType == ASM;
        }

        /**
         * Converts the AOTRC output type to an LLVM codegen file type enum
         * @return llvm output type
         */
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

        /**
         * Gets the aotrc output type
         * @return
         */
        OutputType getOutputType() const {
            return outputType;
        }

        /**
         * Gets a list of definition files to compile
         * @return list of paths to files to compile
         */
        const std::vector<std::string> &getInputFilePaths() const {
            return inputFilePaths;
        }

        /**
         * Get a path to the system linker
         * @return path to system linker
         */
        const std::string &getLinkerPath() const {
            return linkerPath;
        }

        /**
         * Get a path to the system archiver
         * @return
         */
        const std::string &getArPath() const {
            return arPath;
        }

        /**
         * Determine if a shared library should be built via the system linker
         * @return true if shared library build should be skipped
         */
        bool skipBuildShared() const {
            return this->skipShared;
        }

        /**
         * Determine if a static library should be built via the system archiver
         * @return true if static library build should be skipped
         */
        bool skipBuildArchive() const {
            return this->skipArchive;
        }

        /**
         * If rendering graphviz for the FA, gives which FAs should be rendered
         * @return
         */
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
