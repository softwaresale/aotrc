//
// Created by charlie on 12/3/21.
//

#include "dfa.h"

#include <unordered_set>
#include <unordered_map>
#include <queue>

struct StateSetHash {
public:
    std::size_t operator()(const std::unordered_set<unsigned int> &states) const noexcept {
        std::size_t hash = 0;
        for (const auto &state : states) {
            unsigned int x = state;
            x ^= x >> 17;
            x *= 0xed5ad4bb;
            x ^= x >> 11;
            x *= 0xac4c1b51;
            x ^= x >> 15;
            x *= 0x31848bab;
            x ^= x >> 14;

            hash += x;
        }

        return hash;
    }
};

/**
 * Determinize an NFA
 * @param nfa fa to determinize
 */
aotrc::fa::DFA::DFA(const aotrc::fa::NFA &nfa)
: TransitionTable() {
    // Copy over the group count
    this->setGroupCount(nfa.getGroupCount());

    // Map that holds the fa states to dfa states (the key is a set of fa states that corresponds to a single state
    // in the dfa's transition table)
    std::unordered_map<std::unordered_set<unsigned int>, unsigned int, StateSetHash> dfaStateTranslations;
    // Nfa state queue that holds states to be processed, starting with 0
    std::queue<std::unordered_set<unsigned int>> dfaStateQueue;
    // Start with epsilon closure of start state
    auto firstDfaState = nfa.epsilonClosure(nfa.startState());

    // add a state in this transition table for the first dfa state
    auto dfaStartState = this->addState();
    dfaStateTranslations[firstDfaState] = dfaStartState;
    if (firstDfaState.find(nfa.stopState()) != firstDfaState.end()) {
        this->acceptStates.insert(dfaStartState);
    }

    // push the first dfa state onto the stack
    dfaStateQueue.push(firstDfaState);
    // Grab the language for the NFA
    auto nfaLangChars = nfa.languageChars();

    while (!dfaStateQueue.empty()) {
        // Get the first state
        auto state = dfaStateQueue.front();
        dfaStateQueue.pop();

        // For each range in the language, create a new dfa state
        for (const auto &langChar : nfaLangChars) {
            // Create a new state
            auto unclosedDfaState = nfa.move(state, langChar);
            auto newDfaState = nfa.epsilonClosure(unclosedDfaState);
            // Filter out empty transitions
            if (newDfaState.empty()) {
                continue;
            }

            // See if we need to add a new state to this transition table
            unsigned int existingDFAState;
            auto existingDFAStateIter = dfaStateTranslations.find(newDfaState);
            if (existingDFAStateIter != dfaStateTranslations.end()) {
                // We already have a state
                existingDFAState = existingDFAStateIter->second;
            } else {
                // Otherwise, we need to add a new state
                existingDFAState = this->addState();
                dfaStateTranslations[newDfaState] = existingDFAState;

                if (newDfaState.find(nfa.stopState()) != newDfaState.end()) {
                    this->acceptStates.insert(existingDFAState);
                }

                // Add the state to the queue
                dfaStateQueue.push(newDfaState);
            }

            // Get the tag closure between the two as well
            std::unordered_set<int> tags = nfa.tagClosure(state, unclosedDfaState, langChar);
            auto newEdge = Edge { Range(langChar) };
            for (const auto &tag : tags) {
                newEdge.addTag(tag);
            }

            // make an edge in this transition table
            this->addEdge(dfaStateTranslations[state], existingDFAState, newEdge);
        }
    }

    // Optimize edges
    // TODO it is slightly redundant to do this here and above
    for (unsigned int state = 0; state < this->stateCount(); state++) {
        for (auto &[dest, edge] : this->transitions[state]) {
            edge.optimizeRanges();
        }
    }
}

bool aotrc::fa::DFA::simulate(const std::string &subject) const {
    auto startState = this->getStartState();
    std::deque<std::pair<unsigned int, std::string::const_iterator>> traversal_stack;
    traversal_stack.emplace_back(startState, subject.begin());

    while (!traversal_stack.empty()) {
        // Get the top
        auto [state, subjectPos] = traversal_stack.back();
        traversal_stack.pop_back();

        // If we're at the end in an accept state, accept
        if (subjectPos == subject.end() && this->isAcceptState(state)) {
            return true;
        }

        // Get the outgoing transitions
        for (const auto &[dest, edge] : this->transitions[state]) {
            if (edge.accept(*subjectPos)) {
                traversal_stack.emplace_back(dest, subjectPos + 1);
            }
        }
    }

    return false;
}
