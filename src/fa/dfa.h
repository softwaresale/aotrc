//
// Created by charlie on 12/3/21.
//

#ifndef _DFA_H
#define _DFA_H

#include "transition_table.h"
#include "nfa.h"

namespace aotrc::fa {
    class DFA : public TransitionTable {
    public:
        explicit DFA(const NFA &nfa);

        const std::unordered_set<unsigned int>& getAcceptStates() const noexcept {
            return this->acceptStates;
        }

    private:
        std::unordered_set<unsigned int> acceptStates;
    };
}

#endif //_DFA_H
