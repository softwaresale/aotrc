//
// Created by charlie on 12/2/21.
//

#include "transition_table.h"

#include <algorithm>
#include <atomic>
#include <limits>

unsigned int aotrc::fa::TransitionTable::addState() {
    // Add a new state
    this->transitions.emplace_back();
    return this->transitions.size() - 1;
}

bool aotrc::fa::TransitionTable::addEdge(unsigned int source, unsigned int dest, const aotrc::fa::Edge &edge) {
    // Make sure the edges are valid
    if (source >= this->transitions.size() || dest >= this->transitions.size())
        return false;

    // Set the edge
    this->transitions[source][dest] = edge;

    // Success
    return true;
}

bool aotrc::fa::TransitionTable::addEdge(unsigned int source, unsigned int dest, aotrc::fa::Edge &&edge) {
    // Make sure the edges are valid
    if (source >= this->transitions.size() || dest >= this->transitions.size())
        return false;

    // Merge the new edge with the existing edge
    // If the existing edge is empty, it'll just copy in everything from edge
    this->transitions[source][dest].merge(std::move(edge));

    // Success
    return true;
}

unsigned int aotrc::fa::TransitionTable::addState(unsigned int key) {
    // If it's already contained, done
    if (key < this->transitions.size())
        return key;

    // Figure out how many states need to be added
    unsigned int statesToAdd = key - this->transitions.size() + 1;
    for (unsigned int i = 0; i < statesToAdd; i++) {
        this->transitions.emplace_back();
    }

    return key;
}

std::unordered_map<unsigned int, aotrc::fa::Edge> &aotrc::fa::TransitionTable::operator[](unsigned int state) {
    // If the state is already included, then add it
    if (!this->hasState(state)) {
        // Otherwise, add this particular state
        this->addState(state);
    }

    // MatchFunctionState now is guaranteed to exist (more or less)
    return this->transitions[state];
}

std::ostream &aotrc::fa::operator<<(std::ostream &os, const aotrc::fa::TransitionTable &table) {
    for (unsigned int i = 0; i < table.getTransitions().size(); i++) {
        os << "[" << i << ": ";
        for (const auto &transition : table.getTransitions()[i]) {
            os << "(" << transition.first << "," << transition.second << "), ";
        }
        os << "]" << std::endl;
    }
    return os;
}

aotrc::fa::RangeSet aotrc::fa::TransitionTable::language() const {
    RangeSet ranges;
    for (const auto &transition : this->transitions) {
        for (const auto &item : transition) {
            for (const auto &range : item.second.getRanges()) {
                ranges.insert(range);
            }
        }
    }

    return ranges;
}

std::unordered_set<unsigned int>
aotrc::fa::TransitionTable::move(unsigned int state, const aotrc::fa::Range &range) const {
    std::unordered_set<unsigned int> states;
    auto stateTransitions = this->transitions[state];
    for (const auto &trans : stateTransitions) {
        const auto ranges = trans.second.getRanges();
        for (const auto &transRange : ranges) {
            if (transRange == range) {
                states.insert(trans.first);
                break;
            }
        }
    }

    return states;
}

std::unordered_set<unsigned int>
aotrc::fa::TransitionTable::move(const std::unordered_set<unsigned int> &states, const aotrc::fa::Range &range) const {
    std::unordered_set<unsigned int> destinations;
    for (const auto &state : states) {
        auto stateMovements = this->move(state, range);
        std::set_union(destinations.cbegin(),  destinations.cend(), stateMovements.cbegin(),  stateMovements.cend(), std::inserter(destinations, destinations.begin()));
    }

    return destinations;
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
            } else ++second; // Otherwise, do nothing and check the next one
        }
    }

    this->rangesOptimized = true;
}

aotrc::fa::Edge aotrc::fa::Edge::complement() {
    this->optimizeRanges();

    // Make a vector of all values from [char_min, char_max]
    std::vector<unsigned char> allCharacters;
    // unsigned int range = std::numeric_limits<char>::max() - std::numeric_limits<char>::min();
    for (unsigned char i = std::numeric_limits<unsigned char>::min(); i < std::numeric_limits<unsigned char>::max(); i++) {
        allCharacters.push_back(i);
    }

    // For each range, remove that range from the allCharacters vector
    for (const auto &range : this->ranges) {
        auto lowerBound = allCharacters.begin() + range.lower;
        auto upperBound = allCharacters.begin() + range.upper;
        allCharacters.erase(lowerBound, upperBound);
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
            lastLowerBound = lastUpperBound + 1;

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
