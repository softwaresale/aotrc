//
// Created by charlie on 12/2/21.
//

#ifndef _NFA_H
#define _NFA_H

#include "transition_table.h"

namespace aotrc::fa {
    /**
     * Represents a non-deterministic finite automaton
     */
    class NFA : public TransitionTable {
    public:
        NFA()
        : TransitionTable() {
        }

        inline unsigned int startState() const {
            return 0; // This is always 0
        }

        inline unsigned int stopState() const {
            return this->transitions.size() - 1;
        }

        inline bool isAcceptState(unsigned int state) const override {
            return state == this->stopState();
        }

        /**
         * Set of all states accessible from starting state via epsilon transitions
         * @param state starting states
         * @return set of states accessible by this state
         */
        std::unordered_set<unsigned int> epsilonClosure(unsigned int state) const;

        std::unordered_set<unsigned int> epsilonClosure(const std::unordered_set<unsigned int> &states) const;
    };
}

#endif //_NFA_H
