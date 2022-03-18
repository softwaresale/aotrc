//
// Created by charlie on 1/5/22.
//

#include <limits>
#include <algorithm>
#include "transition_edge.h"

std::ostream &aotrc::fa::operator<<(std::ostream &os, const aotrc::fa::Range &range) {
    os << '[';
    if (!std::isalnum(range.lower)) {
        auto numVal = static_cast<unsigned int>(range.lower);
        os << '\\' << numVal;
    } else {
        os << range.lower;
    }
    os << ',';
    if (!std::isalnum(range.upper)) {
        auto numVal = static_cast<unsigned int>(range.upper);
        os << '\\' << numVal;
    } else {
        os << range.upper;
    }
    os << ']';
    return os;
}

std::ostream &aotrc::fa::operator<<(std::ostream &os, const aotrc::fa::Edge &edge) {
    if (edge.epsilon()) {
        os << "$";
    } else {
        os << "{";
        for (const auto &range : edge.getRanges()) {
            os << range;
        }
        os << "}";
    }

    return os;
}

void aotrc::fa::Edge::optimizeRanges() {
    if (rangesOptimized)
        return;

    // Iterate over the ranges twice. If any two have any overlap, collapse the second one into the first one
    for (auto first = this->ranges.begin(); first != this->ranges.end(); ++first) {
        for (auto second = first + 1; second != this->ranges.end();) {
            if (second->lower > first->lower && second->upper < first->upper) {
                // If second fits inside of first, remove it
                second = this->ranges.erase(second);
            } else if (first->lower > second->lower && first->upper < second->upper) {
                // First fits in second, then set first's values to second's and remove second
                first->lower = second->lower;
                first->upper = second->upper;
                second = this->ranges.erase(second);
            } else if (first->upper > second->lower && first->upper < second->upper) {
                // first's upper bound and second's lower bound overlap, so merge them
                first->upper = second->upper;
                second = this->ranges.erase(second);
            } else if (second->upper > first->lower && second->upper < first->upper) {
                // second's upper bound and first's lower bound overlap, so merge them
                first->lower = second->lower;
                second = this->ranges.erase(second);
            } else if (first->upper + 1 == second->lower) {
                // These two are adjacent on the left
                first->upper = second->upper;
                second = this->ranges.erase(second);
            } else if (second->upper + 1 == first->lower) {
                // These two are adjacent on the other side
                first->lower = second->lower;
                second = this->ranges.erase(second);
            } else ++second; // Otherwise, do nothing and check the next one
        }
    }

    this->rangesOptimized = true;
}

static std::vector<char> generateAllCharacters() {
    std::vector<char> allChars(std::numeric_limits<char>::max());
    for (char c = 0; c < std::numeric_limits<char>::max(); c++) {
        allChars[c] = c;
    }

    return allChars;
}

aotrc::fa::Edge aotrc::fa::Edge::complement() {
    this->optimizeRanges();

    // Make a vector of all values from [char_min, char_max]
    auto allCharacters = generateAllCharacters();

    // For each range, remove that range from the allCharacters vector
    for (const auto &range : this->ranges) {
        auto inRange = [&range](const char c) {
            return range.lower <= c && c <= range.upper;
        };
        auto start = std::remove_if(allCharacters.begin(), allCharacters.end(), inRange);
        allCharacters.erase(start, allCharacters.end());
    }

    // Build ranges out of all the characters by clustering them into adjacent clumps
    unsigned int lastLowerBound = 0;
    unsigned int lastUpperBound = 0;
    std::vector<Range> complementedRanges;
    for (auto it = allCharacters.begin(); it != allCharacters.end() - 1; ++it) {
        if ((*it) + 1 == *(it + 1)) {
            // It is adjacent to the next, so increment the upper bound
            lastUpperBound++;
        } else {
            // These are not adjacent, so build a range
            complementedRanges.emplace_back(lastLowerBound, lastUpperBound);
            lastLowerBound = static_cast<unsigned char>(*(it + 1));
            lastUpperBound = lastLowerBound;
        }
    }

    // Might have to do one more
    complementedRanges.emplace_back(lastLowerBound, lastUpperBound);

    // Add all the edges
    Edge complementedEdge;
    for (auto &range : complementedRanges) {
        complementedEdge.addRange(range);
    }
    complementedEdge.optimizeRanges();

    return complementedEdge;
}

void aotrc::fa::Edge::addRange(aotrc::fa::Range newRange) {
    // We want to see if we can fold this range in with another
    bool placed = false;
    for (auto &range : this->ranges) {
        if (newRange.lower >= range.lower && newRange.upper <= range.upper) {
            // If new range fully fits within, just remove it and be done
            // 0-9 + 1-2 ==> 0-9
            return;
        } else if (newRange.upper == (range.lower - 1)) {
            // If the new range is directly adjacent to another range, then add them
            // 5-9 + 0-4 ==> 0-9
            range.lower = newRange.lower;
            placed = true;
            break;
        } else if (newRange.lower == (range.upper + 1)) {
            // 0-4 + 5-9 ==> 0-9
            range.upper = newRange.upper;
            placed = true;
            break;
        } else if (newRange.upper > range.lower && newRange.upper < range.upper) {
            // If upper bound intersects lower bound, change lower bound
            // 3-6 + 2-5 ==> 2-6
            range.lower = newRange.lower;
            placed = true;
            break;
        } else if (newRange.lower < range.upper && newRange.lower > range.lower) {
            // if lower bound intersects upper bound, change upper bound
            // 2-5 + 3-6 ==> 2-6
            range.upper = newRange.upper;
            placed = true;
            break;
        } else if (newRange.lower < range.lower && newRange.upper > range.upper) {
            // new range is just larger that this current range, swap em
            // 4-6 + 0-9 ==> 0-9
            range.lower = newRange.lower;
            range.upper = newRange.upper;
            placed = true;
            break;
        }
    }

    if (!placed)
        this->ranges.push_back(newRange);

    // Things have been added. Might not need this any more...
    this->rangesOptimized = false;
}

void aotrc::fa::Edge::addChar(char c) {
    Range newRange(c);
    this->addRange(newRange);
}

bool aotrc::fa::Edge::accept(char c) const {
    return std::any_of(ranges.cbegin(), ranges.cend(),
                       [c](const Range &range) { return range.lower <= c && c <= range.upper; });
}

