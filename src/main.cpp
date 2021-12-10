#include <iostream>

#include <llvm/Support/InitLLVM.h>

#include "parser/regex_parser.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"
#include "args_parser.h"
#include "compiler/code_gen.h"
#include "input/aotrc_input_parser.h"

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
            aotrc::fa::DFA dfa(nfa);

            // Make a match function out of the dfa
            aotrc::compiler::MatchFunction matchFunction(std::move(dfa), regexDef.label, module, ctx);

            // Compile the match function
            matchFunction.build();
        }
    }

    std::cout << "Generating code..." << std::endl;
    aotrc::compiler::CodeGen codeGen;

    for (const auto &module : ctx->getModules()) {
        // Create the file name
        std::string filename = module.first + '.' + (argsParser.getOutputType() == llvm::CGFT_AssemblyFile ? 's' : 'o');
        auto success = codeGen.compileModule(module.second, filename, argsParser.getOutputType());
        if (!success) {
            throw std::runtime_error("Failed to compile module " + module.first);
        }

        codeGen.generateHeader(module.second, module.first + ".h");
    }

    return 0;
}
