//
// Created by charlie on 12/2/21.
//

#include "nfa.h"

#include <queue>
#include <algorithm>

std::unordered_set<unsigned int> aotrc::fa::NFA::epsilonClosure(unsigned int state) const {
    // Start with this state
    std::unordered_set<unsigned int> states;

    std::queue<unsigned int> traversalQueue;
    traversalQueue.push(state);

    while (!traversalQueue.empty()) {
        unsigned int currentState = traversalQueue.front();
        traversalQueue.pop();

        // Add to visited states
        states.insert(currentState);
        for (const auto &transition : this->transitions[currentState]) {
            // For all epsilon transitions, add the destination to the queue
            if (transition.second.epsilon()) {
                traversalQueue.push(transition.first);
            }
        }
    }

    return states;
}

std::unordered_set<unsigned int> aotrc::fa::NFA::epsilonClosure(const std::unordered_set<unsigned int> &states) const {
    // states copies over
    std::unordered_set<unsigned int> closureStates = states;

    for (const auto &state : states) {
        auto foundStates = this->epsilonClosure(state);
        // Union the two sets
        std::set_union(closureStates.begin(),  closureStates.end(), foundStates.begin(),  foundStates.end(), std::inserter(closureStates, closureStates.begin()));
    }

    return closureStates;
}
