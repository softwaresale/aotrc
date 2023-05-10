#include <llvm/Support/InitLLVM.h>
#include <unistd.h>
#include <iostream>

#include "args_parser.h"
#include "input/aotrc_input_parser.h"
#include "src/parser/regex_parser.h"
#include "src/fa/dfa.h"
#include "src/compiler/aotrc_compiler.h"
#include "src/compiler/linker.h"
#include "src/fa/graphviz_renderer.h"

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

    aotrc::compiler::AotrcCompiler compiler;
    aotrc::compiler::Linker linker("/usr/bin/ld", "/usr/bin/ar");
    for (const auto &inputFilePath : argsParser.getInputFilePaths()) {
        aotrc::input::AotrcInputParser inputFileParser(inputFilePath);

        for (const auto &[moduleName, regexDefs] : inputFileParser.getModules()) {
            for (const auto &regexDef : regexDefs) {
                // build the NFA and DFA for the given regex
                auto nfa = aotrc::parser::parseRegex(regexDef.pattern);
                aotrc::fa::DFA dfa(nfa);
                aotrc::fa::graphvizRenderOutput(&dfa, regexDef.label, std::cout);
                exit(0);
                compiler.compileCaptureRegex(moduleName, regexDef.label, dfa);

                if (regexDef.genFullMatch) {
                    compiler.compileRegex(moduleName, regexDef.label, dfa);
                }

                if (regexDef.genSubMatch) {
                    compiler.compileSubMatchRegex(moduleName, regexDef.label, dfa);
                }

                if (regexDef.genSearch) {
                    compiler.compileSearchRegex(moduleName, regexDef.label, dfa);
                }

                // Generate a pattern function
                compiler.generatePatternFunc(moduleName, regexDef.label, regexDef.pattern);
            }

            compiler.emitHeaderFile(moduleName);

            // If type is HIR, then we don't need to output anything
            if (argsParser.getOutputType() == aotrc::OutputType::HIR) {
                continue;
            }

            if (argsParser.getOutputType() == aotrc::OutputType::OBJ) {
                compiler.emitObjectFile(moduleName);
            } else if (argsParser.getOutputType() == aotrc::OutputType::ASM) {
                compiler.emitAssembly(moduleName);
            } else if (argsParser.getOutputType() == aotrc::OutputType::SHARED) {
                auto objectFilePath = compiler.emitObjectFile(moduleName);
                std::string sharedLibraryName = moduleName + ".so";
                int ret = linker.linkShared({ objectFilePath }, sharedLibraryName);
                if (ret != 0) {
                    throw std::runtime_error("Error encountered while invoking ld: " + std::to_string(ret));
                }
                ret = unlink(objectFilePath.c_str());
                if (ret) {
                    throw std::runtime_error("Error encountered while invoking unlink: " + std::to_string(ret));
                }
            } else if (argsParser.getOutputType() == aotrc::OutputType::STATIC) {
                auto objectFilePath = compiler.emitObjectFile(moduleName);
                std::string sharedLibraryName = moduleName + ".a";
                int ret = linker.linkStatic({ objectFilePath }, sharedLibraryName);
                if (ret != 0) {
                    throw std::runtime_error("Error encountered while invoking ar: " + std::to_string(ret));
                }
                ret = unlink(objectFilePath.c_str());
                if (ret) {
                    throw std::runtime_error("Error encountered while invoking unlink: " + std::to_string(ret));
                }
            } else {
                compiler.emitIr(moduleName);
            }
        }
    }

    return 0;
}
