//
// Created by charlie on 12/2/21.
//

#include "transition_table.h"

#include <algorithm>

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

    // Set the edge
    this->transitions[source][dest] = edge;

    // Success
    return true;
}

unsigned int aotrc::fa::TransitionTable::addState(unsigned int key) {
    // If it's already contained, done
    if (key < this->transitions.size())
        return key;

    // Figure out how many states need to be added
    unsigned int statesToAdd = key - this->transitions.size() + 1;
    for (int i = 0; i < statesToAdd; i++) {
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
    for (int i = 0; i < table.transitions.size(); i++) {
        os << "[" << i << ": ";
        for (const auto &transition : table.transitions[i]) {
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
