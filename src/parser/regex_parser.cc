//
// Created by charlie on 12/3/21.
//

#include "regex_parser.h"

#include "re_parser_driver.h"

aotrc::fa::NFA aotrc::parser::parse_regex(const std::string &pattern) {
    RegexParserDriver driver;
    return driver.parse(pattern);
}
