//
// Created by charlie on 12/3/21.
//

#ifndef _DFA_H
#define _DFA_H

#include <algorithm>
#include "transition_table.h"
#include "nfa.h"

namespace aotrc::fa {
    /**
     * Represents a deterministic finite automaton
     */
    class DFA : public TransitionTable {
    public:
        explicit DFA(const NFA &nfa);

        // Empty
        DFA() : TransitionTable() {}

        /**
         * Determines if a given state is an accept state
         * @param state a dfa state
         * @return true if the state is an accept state
         */
        inline bool isAcceptState(unsigned int state) const override {
            return std::count(this->acceptStates.cbegin(), this->acceptStates.cend(), state) > 0;
        }

        /**
         * Gets the set of accept states
         * @return
         */
        const std::unordered_set<unsigned int>& getAcceptStates() const noexcept {
            return this->acceptStates;
        }

        unsigned int getStartState() const {
            return 0;
        }

        bool simulate(const std::string &subject) const;

    private:
        std::unordered_set<unsigned int> acceptStates;
    };
}

#endif //_DFA_H
