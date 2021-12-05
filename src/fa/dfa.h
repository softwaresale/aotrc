//
// Created by charlie on 12/3/21.
//

#ifndef _DFA_H
#define _DFA_H

#include <algorithm>
#include "transition_table.h"
#include "nfa.h"

namespace aotrc::fa {
    class DFA : public TransitionTable {
    public:
        explicit DFA(const NFA &nfa);

        bool isAcceptState(unsigned int state) const {
            return std::count(this->acceptStates.cbegin(), this->acceptStates.cend(), state) > 0;
        }
        const std::unordered_set<unsigned int>& getAcceptStates() const noexcept {
            return this->acceptStates;
        }

        unsigned int getStartState() const {
            return 0;
        }

    private:
        std::unordered_set<unsigned int> acceptStates;
    };
}

#endif //_DFA_H
