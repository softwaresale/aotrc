//
// Created by charlie on 12/2/21.
//

#ifndef _NFA_BUILDERS_H
#define _NFA_BUILDERS_H

#include "nfa.h"

#include <string>

namespace aotrc::fa::nfa_builders {
    NFA epsilon();
    NFA singleChar(char c);
    NFA characterClass(const std::vector<Range> &ranges, bool negated);
    NFA literal(const std::string &literal);
    NFA concat(NFA &&left, NFA &&right);
    NFA alternation(NFA &&left, NFA &&right);
    NFA star(NFA &&nfa);
    NFA plus(NFA &&nfa);
    NFA questionMark(NFA &&nfa);
}

#endif //_NFA_BUILDERS_H
