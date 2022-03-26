#include <llvm/Support/InitLLVM.h>

#include "args_parser.h"
#include "input/aotrc_input_parser.h"
#include "compiler/program.h"
#include "compiler/compiler.h"

#if 0
static std::string getOutputFileName(const std::string &moduleName, aotrc::OutputType outputType) {
    std::string filename = moduleName + '.';
    switch (outputType) {
        case aotrc::OutputType::ASM:
            filename += 's';
            break;
        case aotrc::OutputType::OBJ:
            filename += 'o';
            break;
        case aotrc::OutputType::IR:
            filename += "ir";
            break;
        default:
            break;
    }

    return filename;
}
#endif

int main(int argc, char **argv) {
    llvm::InitLLVM initLLVM(argc, argv);
    aotrc::ArgsParser argsParser(argc, argv);

    if (argsParser.hasHelp()) {
        aotrc::ArgsParser::displayHelp();
        return 0;
    }

    if (argsParser.hasVersion()) {
        aotrc::ArgsParser::displayVersion();
        return 0;
    }

    aotrc::compiler::Compiler compiler;
    for (const auto &inputFilePath : argsParser.getInputFilePaths()) {
        aotrc::input::AotrcInputParser inputFileParser(inputFilePath);

        for (const auto &[moduleName, regexDefs] : inputFileParser.getModules()) {
            for (const auto &regexDef : regexDefs) {
                if (regexDef.genFullMatch) {
                    compiler.compileRegex(moduleName, regexDef.label, regexDef.pattern);
                }

                if (regexDef.genSubMatch) {
                    compiler.compileSubmatchRegex(moduleName, regexDef.label, regexDef.pattern);
                }
            }

            compiler.emitHeaderFile(moduleName);
            if (argsParser.getOutputType() == aotrc::OutputType::OBJ) {
                compiler.emitObjectFile(moduleName);
            } else if (argsParser.getOutputType() == aotrc::OutputType::ASM) {
                compiler.emitAssembly(moduleName);
            } else {
                compiler.emitIr(moduleName);
            }
        }
    }

    return 0;
}
