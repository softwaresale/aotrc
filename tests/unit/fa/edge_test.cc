//
// Created by charlie on 3/17/22.
//

#include <gtest/gtest.h>
#include "src/fa/transition_edge.h"

TEST(Edge, Complement_EverythingExceptCharacter) {
    // Just a
    aotrc::fa::Edge edge({ {'a'} });
    auto complement = edge.complement();
    auto ranges = complement.getRanges();
    // There should be two ranges: everything above and everything below
    ASSERT_EQ(ranges.size(), 2);
    // Ranges should be [0, a), (a, max]
    ASSERT_EQ(ranges[0].lower, 0);
    ASSERT_EQ(ranges[0].upper, 'a' - 1);
    ASSERT_EQ(ranges[1].lower, 'a' + 1);
    ASSERT_EQ(ranges[1].upper, std::numeric_limits<char>::max() - 1);
}

TEST(Edge, Complement_EverythingExceptRange) {
    aotrc::fa::Edge edge({ {'a', 'z'} });
    auto complement = edge.complement();
    auto ranges = complement.getRanges();
    // There should be two ranges: everything above and everything below
    ASSERT_EQ(ranges.size(), 2);
    // Ranges should be [0, a), (a, max]
    ASSERT_EQ(ranges[0].lower, 0);
    ASSERT_EQ(ranges[0].upper, 'a' - 1);
    ASSERT_EQ(ranges[1].lower, 'z' + 1);
    ASSERT_EQ(ranges[1].upper, std::numeric_limits<char>::max() - 1);
}

TEST(Edge, Complement_EverythingExceptRangeAndCharacter) {
    aotrc::fa::Edge edge({ {'a', 'z'}, {'B'} });
    auto complement = edge.complement();
    auto ranges = complement.getRanges();
    // There should be two ranges: everything above and everything below
    ASSERT_EQ(ranges.size(), 3);
    // Ranges should be [0, a), (a, max]
    ASSERT_EQ(ranges[0].lower, 0);
    ASSERT_EQ(ranges[0].upper, 'B' - 1);
    ASSERT_EQ(ranges[1].lower, 'B' + 1);
    ASSERT_EQ(ranges[1].upper, 'a' - 1);
    ASSERT_EQ(ranges[2].lower, 'z' + 1);
    ASSERT_EQ(ranges[2].upper, std::numeric_limits<char>::max() - 1);
}

TEST(Edge, Complement_TakeWholeRangeInChunks) {
    aotrc::fa::Edge edge({ {'a', 'y'}, {'z'} });
    auto complement = edge.complement();
    auto ranges = complement.getRanges();
    // There should be two ranges: everything above and everything below
    ASSERT_EQ(ranges.size(), 2);
    // Ranges should be [0, a), (a, max]
    ASSERT_EQ(ranges[0].lower, 0);
    ASSERT_EQ(ranges[0].upper, 'a' - 1);
    ASSERT_EQ(ranges[1].lower, 'z' + 1);
    ASSERT_EQ(ranges[1].upper, std::numeric_limits<char>::max() - 1);
}

TEST(Edge, Optimize_CombineInternal) {
    aotrc::fa::Edge internal { {'a', 'z'}, {'m', 'n'} };

    // Before there are two
    ASSERT_EQ(internal.getRanges().size(), 2);
    // Optimize
    internal.optimizeRanges();
    // Now there should be one from 'a'-'z'
    ASSERT_EQ(internal.getRanges().size(), 1);
    ASSERT_EQ(internal.getRanges().begin()->lower, 'a');
    ASSERT_EQ(internal.getRanges().begin()->upper, 'z');
}

TEST(Edge, Optimize_CombineLowerOverlap) {
    aotrc::fa::Edge internal { {'a', 'x'}, {'m', 'z'} };

    // Before there are two
    ASSERT_EQ(internal.getRanges().size(), 2);
    // Optimize
    internal.optimizeRanges();
    // Now there should be one from 'a'-'z'
    ASSERT_EQ(internal.getRanges().size(), 1);
    ASSERT_EQ(internal.getRanges().begin()->lower, 'a');
    ASSERT_EQ(internal.getRanges().begin()->upper, 'z');
}

TEST(Edge, Optimize_CombineAdjacent) {
    aotrc::fa::Edge internal { {'a', 'm'}, {'n', 'z'} };

    // Before there are two
    ASSERT_EQ(internal.getRanges().size(), 2);
    // Optimize
    internal.optimizeRanges();
    // Now there should be one from 'a'-'z'
    ASSERT_EQ(internal.getRanges().size(), 1);
    ASSERT_EQ(internal.getRanges().begin()->lower, 'a');
    ASSERT_EQ(internal.getRanges().begin()->upper, 'z');
}
