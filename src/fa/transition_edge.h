//
// Created by charlie on 1/5/22.
//

#ifndef _TRANSITION_EDGE_H
#define _TRANSITION_EDGE_H

#include <cstddef>
#include <sstream>
#include <vector>
#include <unordered_set>

namespace aotrc::fa {
    /**
     * Represents a basic character range
     */
    struct Range {
    public:
        Range(unsigned char lower, unsigned char upper)
        : lower(lower)
        , upper(upper) {
        }

        explicit Range(unsigned char single)
        : lower(single)
        , upper(single) {
        }

        bool operator==(const Range &otherRange) const noexcept {
            return otherRange.upper == this->upper && otherRange.lower == this->lower;
        }

        unsigned char lower;
        unsigned char upper;
    };

    struct RangeHash {
        std::size_t operator()(const aotrc::fa::Range &range) const noexcept {
            std::stringstream ss;
            ss << range.lower << range.upper;
            std::hash<std::string> string_hash;
            return string_hash(ss.str());
        }
    };

    struct RangeEquals {
        constexpr bool operator()(const aotrc::fa::Range &leftRange, const aotrc::fa::Range &rightRange) const noexcept {
            return leftRange.lower == rightRange.lower && leftRange.upper == rightRange.upper;
        }
    };
    using RangeSet = std::unordered_set<Range, RangeHash, RangeEquals>;

    /**
     * An edge that lives within a transition table. An edge holds a collection of ranges. Each range specifies what
     * characters can be used to traverse this edge.
     */
    class Edge {
    public:
        Edge()
        : rangesOptimized(false) {  }

        Edge(std::initializer_list<Range> ranges)
        : ranges(ranges)
        , rangesOptimized(false) { }

        /**
         * Collapse any ranges with overlap in order to minimize the number of ranges involved
         */
        void optimizeRanges();

        /**
         * Adds a range of characters to this edge. Tries to fold newRange into an existing range
         * @param newRange range to add to the edge
         */
        void addRange(Range newRange);

        /**
         * tries adding a single character range to the edge. Behaves the same as addRange where new range
         * is a range of a single character
         * @param c character to add
         */
        void addChar(unsigned char c);

        const std::vector<Range> &getRanges() const {
            return ranges;
        }

        inline bool epsilon() const {
            return this->ranges.empty();
        }

        void merge(Edge &&other) {
            for (auto &range : other.ranges) {
                this->addRange(range);
            }
            this->rangesOptimized = false;
        }

        void merge(const Edge &other) {
            for (auto &range : other.ranges) {
                this->addRange(range);
            }
            this->rangesOptimized = false;
        }

        Edge complement();

    private:
        std::vector<Range> ranges;
        bool rangesOptimized;
    };

    std::ostream &operator<<(std::ostream &os, const Range &range);
    std::ostream &operator<<(std::ostream &os, const Edge &edge);
}

#endif //_TRANSITION_EDGE_H
