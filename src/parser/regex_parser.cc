//
// Created by charlie on 12/3/21.
//

#include "regex_parser.h"

#include "re_parser_driver.h"

aotrc::fa::NFA aotrc::parser::parseRegex(const std::string &pattern) {
    RegexParserDriver driver;
    return driver.parse(pattern);
}

aotrc::fa::DFA aotrc::parser::parseRegexDFA(const std::string &pattern) {
    auto nfa = parseRegex(pattern);
    return aotrc::fa::DFA(nfa);
}
