//
// Created by charlie on 3/17/22.
//

#include "src/parser/regex_parser.h"
#include <gtest/gtest.h>

TEST(TestQuants, ParseStartCorrectly) {
    auto nfa = aotrc::parser::parseRegex("a*");

    // 4 states in a single character star
    ASSERT_EQ(nfa.stateCount(), 4);
    // Edge between two 1 and two should be a
    auto stateOneEdges = nfa.edgesForState(1);
    auto edgeToTwo = stateOneEdges[2];
    ASSERT_EQ(edgeToTwo.getRanges().size(), 1);
    ASSERT_EQ(edgeToTwo.getRanges().begin()->lower, 'a');
}

TEST(TestQuants, ShouldFailIncorrectStar) {
    EXPECT_THROW(aotrc::parser::parseRegex("*a"), std::runtime_error);
}

TEST(TestQuants, ParsePlusCorrectly) {
    auto nfa = aotrc::parser::parseRegex("a+");

    // 4 states in a single character star
    ASSERT_EQ(nfa.stateCount(), 5);
    // Edge between 0 and 1 should be a
    auto startEdges = nfa.edgesForState(0);
    auto edgeToOne = startEdges[1];
    ASSERT_EQ(edgeToOne.getRanges().size(), 1);
    ASSERT_EQ(edgeToOne.getRanges().begin()->lower, 'a');

    // Edge between two and three should be a
    auto stateOneEdges = nfa.edgesForState(2);
    auto edgeToTwo = stateOneEdges[3];
    ASSERT_EQ(edgeToTwo.getRanges().size(), 1);
    ASSERT_EQ(edgeToTwo.getRanges().begin()->lower, 'a');
}

TEST(TestQuants, ShouldFailIncorrectPlus) {
    EXPECT_THROW(aotrc::parser::parseRegex("+a"), std::runtime_error);
}

