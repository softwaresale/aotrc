//
// Created by charlie on 3/18/22.
//

#include <gtest/gtest.h>
#include "src/parser/regex_parser.h"

TEST(ParseNumber, ThreeSeparateDigits) {
    auto nfa = aotrc::parser::parseRegex("123");
    ASSERT_EQ(nfa.stateCount(), 4);
    auto edges = nfa.edgesForState(1);
    auto toTwo = edges[2];
    ASSERT_EQ(toTwo.getRanges().size(), 1);
    ASSERT_EQ(toTwo.getRanges().begin()->lower, '2');
}

TEST(ParseNumber, PickupNumberInQuantifier) {
    auto nfa = aotrc::parser::parseRegex("a{12}");
    // There should be 13 states
    ASSERT_EQ(nfa.stateCount(), 13);
}
