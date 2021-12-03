//
// Created by charlie on 12/2/21.
//

#include "transition_table.h"

unsigned int aotrc::fa::TransitionTable::addState() {
    // Add a new state
    this->transitions.emplace_back();
    return this->transitions.size() - 1;
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

    // State now is guaranteed to exist (more or less)
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
