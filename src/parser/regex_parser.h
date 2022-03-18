//
// Created by charlie on 12/3/21.
//

#ifndef _REGEX_PARSER_H
#define _REGEX_PARSER_H

#include "../fa/nfa.h"

namespace aotrc::parser {
    /**
     * Parse a regex into an NFA
     * @param pattern pattern to parse
     * @return an NFA of the regex
     */
    fa::NFA parseRegex(const std::string &pattern);
}

#endif //_REGEX_PARSER_H
