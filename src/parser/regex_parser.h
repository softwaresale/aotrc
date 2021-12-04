//
// Created by charlie on 12/3/21.
//

#ifndef _REGEX_PARSER_H
#define _REGEX_PARSER_H

#include "../fa/nfa.h"

namespace aotrc::parser {
    fa::NFA parse_regex(const std::string &pattern);
}

#endif //_REGEX_PARSER_H
