#include <iostream>

#include <getopt.h>

#include "parser/regex_parser.h"
#include "fa/nfa.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"
#include "args_parser.h"

int main(int argc, char **argv) {

    aotrc::ArgsParser parser(argc, argv);

    if (parser.hasHelp()) {
        parser.displayHelp();
        return 0;
    }

    // Setup compiler context
    auto ctx = aotrc::compiler::CompilerContext::instance();
    auto defaultModule = ctx->addModule("defaultModule");

    std::vector<aotrc::fa::DFA> dfas;
    for (const auto &regex : parser.getRemainingData()) {
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

    return 0;
}
