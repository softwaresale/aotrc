//
// Created by charlie on 3/17/22.
//

#include <gtest/gtest.h>
#include "src/parser/regex_parser.h"

TEST(TestSpecials, ShouldParseValidHyphen) {
    auto nfa = aotrc::parser::parseRegex("a-z");
    // Should be 4 states, hyphen is a literal here
    ASSERT_EQ(nfa.stateCount(), 4);
    // There should be one edge between states one and two
    auto stateOneEdges = nfa.edgesForState(1);
    auto edges = stateOneEdges[2];
    ASSERT_EQ(edges.getRanges().size(), 1);
    ASSERT_EQ(edges.getRanges().begin()->lower, '-');
}

TEST(TestSpecials, ShouldParseEscapedNewline) {
    auto nfa = aotrc::parser::parseRegex("\\n");
    // Two states
    ASSERT_EQ(nfa.stateCount(), 2);
    // Edge between start and end should only contain a newline character
    auto edge = nfa.edgesForState(nfa.startState()).at(nfa.stopState());
    ASSERT_EQ(edge.getRanges().size(), 1);
    ASSERT_EQ(edge.getRanges().begin()->lower, '\n');
}

TEST(TestSpecials, ShouldNotParseUnescapedNewline) {
    EXPECT_THROW(aotrc::parser::parseRegex("\n"), std::runtime_error);
}

TEST(TestSpecials, ShouldParseEscapedTab) {
    auto nfa = aotrc::parser::parseRegex("\\t");
    // Two states
    ASSERT_EQ(nfa.stateCount(), 2);
    // Edge between start and end should only contain a newline character
    auto edge = nfa.edgesForState(nfa.startState()).at(nfa.stopState());
    ASSERT_EQ(edge.getRanges().size(), 1);
    ASSERT_EQ(edge.getRanges().begin()->lower, '\t');
}
