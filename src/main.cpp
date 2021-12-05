#include <iostream>

#include "parser/regex_parser.h"
#include "fa/nfa.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"
#include "compiler/state_graph.h"

int main() {

    auto regexNFA = aotrc::parser::parse_regex("ab*c");
    aotrc::fa::DFA dfa(regexNFA);
    std::cout << dfa << std::endl;

    auto compilerContext = aotrc::compiler::CompilerContext::instance();
    auto module = compilerContext->addModule("letters");
    aotrc::compiler::MatchFunction matchFunction(std::move(dfa), "lowercase", module, compilerContext);

    matchFunction.build();

    llvm::outs() << *module;

    return 0;
}
