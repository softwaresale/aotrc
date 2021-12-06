#include <iostream>

#include <llvm/Support/InitLLVM.h>

#include "parser/regex_parser.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"
#include "args_parser.h"
#include "compiler/code_gen.h"

int main(int argc, char **argv) {

    llvm::InitLLVM initLLVM(argc, argv);

    aotrc::ArgsParser argsParser(argc, argv);

    if (argsParser.hasHelp()) {
        argsParser.displayHelp();
        return 0;
    }

    // Setup compiler context
    auto ctx = aotrc::compiler::CompilerContext::instance();
    auto defaultModule = ctx->addModule("defaultModule");

    std::vector<aotrc::fa::DFA> dfas;
    for (const auto &regex : argsParser.getRemainingData()) {
        auto nfa = aotrc::parser::parse_regex(regex);
        dfas.emplace_back(nfa);
    }

    std::vector<aotrc::compiler::MatchFunction> matchFunctions;
    int counter = 0;
    for (auto dfa : dfas) {
        matchFunctions.emplace_back(std::move(dfa), "re" + std::to_string(counter++), defaultModule, ctx);
    }

    for (auto &func : matchFunctions) {
        func.build();
    }

    llvm::outs() << *defaultModule;

    std::cout << "Generating code..." << std::endl;
    aotrc::compiler::CodeGen codeGen;

    std::stringstream outputFileName;
    outputFileName << defaultModule->getName().str();
    if (argsParser.getOutputType() == llvm::CGFT_AssemblyFile) {
        outputFileName << ".s";
    } else {
        outputFileName << ".o";
    }
    auto res = codeGen.compileModule(defaultModule, outputFileName.str(), argsParser.getOutputType());
    if (!res) {
        return 1;
    }

    return 0;
}
