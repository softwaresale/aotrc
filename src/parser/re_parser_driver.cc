
#include "re_parser_driver.h"

#include <cstdio>

void aotrc::parser::RegexParserDriver::startScan(const std::string &pattern) {
    load_buffer(pattern.c_str());
}

void aotrc::parser::RegexParserDriver::stopScan() {
    cleanup_buffer();
}

aotrc::fa::NFA aotrc::parser::RegexParserDriver::parse(const std::string &subject) {
    // Start parsing
    this->startScan(subject);
    fa::NFA resulting_nfa;
    aotrc::parser::RegexParser parser(*this, &resulting_nfa);
    // parser.set_debug_level(1);
    auto result = parser();
    if (result == 1) {
        throw std::runtime_error("Could not parse regex");
    }
    this->stopScan();

    return resulting_nfa;
}
