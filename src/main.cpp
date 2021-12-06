#include <iostream>

#include "parser/regex_parser.h"
#include "fa/nfa.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"

int main() {

    auto regexNFA = aotrc::parser::parse_regex("a*");
    aotrc::fa::DFA dfa(regexNFA);

    auto secondRegex = aotrc::parser::parse_regex("ab*c");
    aotrc::fa::DFA secondDFa(secondRegex);

    auto compilerContext = aotrc::compiler::CompilerContext::instance();
    auto module = compilerContext->addModule("letters");
    aotrc::compiler::MatchFunction matchFunction(std::move(dfa), "all_as", module, compilerContext);
    aotrc::compiler::MatchFunction secondMatchFunction(std::move(secondDFa), "absc", module, compilerContext);
    matchFunction.build();
    secondMatchFunction.build();
    llvm::outs() << *module;

    return 0;
}
