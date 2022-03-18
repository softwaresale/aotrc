//
// Created by charlie on 3/16/22.
//

#include <gtest/gtest.h>
#include "src/parser/regex_parser.h"

TEST(RegexParser, parse_basic_literal) {
    auto result = aotrc::parser::parseRegex("a");
    // There should be two states
    ASSERT_EQ(result.stateCount(), 2);
    // There should be one edge
    ASSERT_EQ(result.edgesForState(result.startState()).size(), 1);
    auto &[dest, edge] = *result.edgesForState(result.startState()).begin();
    // Should have one edge going from start to end with 'a'
    ASSERT_EQ(dest, result.stopState());
    ASSERT_EQ(edge.getRanges().size(), 1);
    auto onlyRange = *edge.getRanges().begin();
    ASSERT_EQ(onlyRange.lower, 'a');
}
