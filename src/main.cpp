#include <iostream>

#include "fa/nfa.h"
#include "parser/regex_parser.h"
#include "fa/dfa.h"

int main() {

    auto nfa = aotrc::parser::parse_regex("a*");

    std::cout << nfa << std::endl;
    std::cout << "Determinizing..." << std::endl;
    aotrc::fa::DFA dfa(nfa);
    std::cout << dfa << std::endl;
    for (const auto &state : dfa.getAcceptStates()) {
        std::cout << state << ",";
    }
    std::cout << std::endl;

    return 0;
}
