#include <iostream>

#include <llvm/Support/InitLLVM.h>

#include "parser/regex_parser.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"
#include "args_parser.h"
#include "compiler/code_gen.h"
#include "input/aotrc_input_parser.h"
#include "compiler/shared_lib_linker.h"
#include "compiler/build_pattern_func.h"

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

int main(int argc, char **argv) {

    llvm::InitLLVM initLLVM(argc, argv);

    aotrc::ArgsParser argsParser(argc, argv);

    if (argsParser.hasHelp()) {
        argsParser.displayHelp();
        return 0;
    }

    aotrc::input::AotrcInputParser inputFileParser(argsParser.getInputFilePaths()[0]);

    // Setup compiler context
    auto ctx = aotrc::compiler::CompilerContext::instance();
    for (const auto &moduleDef : inputFileParser.getModules()) {
        auto module = ctx->addModule(moduleDef.first);

        // Build all the regexes
        for (const auto &regexDef : moduleDef.second) {
            // Turn pattern into fa
            aotrc::fa::NFA nfa = aotrc::parser::parse_regex(regexDef.pattern);

            // Make a match function out of the dfa
            if (regexDef.genFullMatch) {
                // NOTE: this is probably not a very good practice... it will likely use a lot of memory
                aotrc::fa::DFA dfa(nfa);
                aotrc::compiler::MatchFunction matchFunction(std::move(dfa), regexDef.label, false, module, ctx);

                // Compile the match function
                matchFunction.build();
            }

            if (regexDef.genSubMatch) {
                // NOTE: this is probably not a very good practice... it will likely use a lot of memory
                aotrc::fa::DFA dfa(nfa);
                aotrc::compiler::MatchFunction matchFunction(std::move(dfa), regexDef.label, true, module, ctx);

                // Compile the match function
                matchFunction.build();
            }

            // Build a pattern global
            // aotrc::compiler::build_pattern_func(regexDef.label, regexDef.pattern, module, ctx);
            if (!aotrc::compiler::build_pattern_func(regexDef.label, regexDef.pattern, module, ctx)) {
                throw std::runtime_error("Error while creating pattern retrieval function");
            }
        }
    }

    aotrc::compiler::CodeGen codeGen(ctx);
    aotrc::compiler::SharedLibLinker libLinker(argsParser.getLinkerPath());

    for (const auto &module : ctx->getModules()) {
        // Create the file name
        std::string filename = getOutputFileName(module.first, argsParser.getOutputType());

        // If we have a code gen type, then compile it
        if (argsParser.hasCodeGenType()) {
            auto success = codeGen.compileModule(module.second, filename, argsParser.outputTypeAsCodeGen());
            if (!success) {
                throw std::runtime_error("Failed to compile module " + module.first);
            }

            // Create a shared lib if it's an object file
            if (argsParser.getOutputType() == aotrc::OutputType::OBJ) {
                std::string libname = "lib" + module.first + ".so";
                libLinker.link(filename, libname, true);
            }

            codeGen.generateHeader(module.second, module.first + ".h");
        } else if (argsParser.getOutputType() == aotrc::OutputType::IR) {
            std::error_code code;
            llvm::raw_fd_ostream dumpFile(filename, code);
            if (code) {
                throw std::runtime_error(code.message());
            }

            // Write the dump file out
            dumpFile << *module.second;
        } else {
            throw std::runtime_error("Invalid output type...");
        }
    }

    return 0;
}
