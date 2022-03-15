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

bool aotrc::fa::NFA::simulate(const std::string &subject) {
    // Get the start state
    unsigned int start = this->startState();
    std::deque<std::pair<unsigned int, std::string::const_iterator>> traversal_stack;
    // Start the traversal_stack with transitions for the current state
    traversal_stack.emplace_back(start, subject.begin());
    while (!traversal_stack.empty()) {
        // Get the top of the stack
        auto [state, subjectPos] = traversal_stack.back();
        traversal_stack.pop_back();

        // If we're at the end and on an accept state, accept
        if (subjectPos == subject.end() && this->isAcceptState(state))
            return true;

        // Get all the edges for this state
        auto stateEdges = this->transitions[state];
        for (const auto &[dest, edge] : stateEdges) {
            if (edge.epsilon()) {
                // If we have an epsilon, then don't bump the position
                traversal_stack.emplace_back(dest, subjectPos);
            } else if (edge.accept(*subjectPos)) {
                // If this edge accepts this character, then bump the position
                traversal_stack.emplace_back(dest, subjectPos + 1);
            }
        }
    }

    // We had nowhere to go, so reject
    return false;
}
