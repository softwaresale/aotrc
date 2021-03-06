//
// Created by charlie on 12/2/21.
//

#include "transition_table.h"
#include "transition_edge.h"

#include <algorithm>
#include <atomic>

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

std::unordered_set<unsigned char> aotrc::fa::TransitionTable::languageChars() const {
    RangeSet langRanges = this->language();
    std::unordered_set<unsigned char> allCharacters;
    for (const auto &range : langRanges) {
        for (unsigned char c = range.lower; c <= range.upper; c++) {
            allCharacters.insert(c);
        }
    }

    return allCharacters;
}

std::unordered_set<unsigned int>
aotrc::fa::TransitionTable::move(unsigned int state, unsigned char c) const {
    std::unordered_set<unsigned int> states;
    auto stateTransitions = this->transitions[state];
    for (const auto &trans : stateTransitions) {
        const auto ranges = trans.second.getRanges();
        for (const auto &transRange : ranges) {
            if (c >= transRange.lower && c <= transRange.upper) {
                states.insert(trans.first);
                break;
            }
        }
    }

    return states;
}

std::unordered_set<unsigned int>
aotrc::fa::TransitionTable::move(const std::unordered_set<unsigned int> &state, unsigned char c) const {
    std::unordered_set<unsigned int> states;
    for (const auto &substate : state) {
        auto substateMoves = this->move(substate, c);
        std::set_union(states.cbegin(),  states.cend(), substateMoves.cbegin(),  substateMoves.cend(), std::inserter(states, states.begin()));
    }

    return states;
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
