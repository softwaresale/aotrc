//
// Created by charlie on 12/2/21.
//

#ifndef _NFA_BUILDERS_H
#define _NFA_BUILDERS_H

#include "nfa.h"

#include <string>

namespace aotrc::fa::nfa_builders {
    /*
     * These functions all build NFAs following Thompson constructions.
     *
     * Also, notice that all NFAs are passed as r-values rather than references or
     * value. The idea behind this is that when passing in a NFA that gets built into
     * another NFA, there is no reason to keep the prior NFA. So, NFA passed in should
     * be used with `std::move`. If you need a copy, make a copy first and then move the copy.
     */
    NFA epsilon();
    NFA singleChar(char c);
    NFA dot();
    NFA characterClass(const std::vector<Range> &ranges, bool negated);
    NFA literal(const std::string &literal);
    NFA concat(NFA &&left, NFA &&right);
    NFA concatMany(std::vector<NFA> &&nfas);
    NFA alternation(std::vector<NFA> &&nfas);
    NFA star(NFA &&nfa);
    NFA plus(NFA &&nfa);
    NFA questionMark(NFA &&nfa);
    NFA numberBounded(NFA &&nfa, unsigned int lower, unsigned int higher);
    NFA numberUnbounded(NFA &&nfa, unsigned int lower);
}

#endif //_NFA_BUILDERS_H
