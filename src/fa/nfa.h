//
// Created by charlie on 12/2/21.
//

#ifndef _NFA_H
#define _NFA_H

#include "transition_table.h"

namespace aotrc::fa {
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

        /**
         * Set of all states accessible from starting state via epsilon transitions
         * @param state starting states
         * @return set of states accessible by this state
         */
        std::unordered_set<unsigned int> epsilonClosure(unsigned int state);

        std::unordered_set<unsigned int> epsilonClosure(const std::unordered_set<unsigned int> &states);
    };
}

#endif //_NFA_H
