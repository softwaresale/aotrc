//
// Created by charlie on 12/2/21.
//

#include "nfa_builders.h"

aotrc::fa::NFA aotrc::fa::nfa_builders::epsilon() {
    aotrc::fa::NFA nfa;
    auto start = nfa.addState();
    auto end = nfa.addState();
    nfa.addEdge(start, end, Edge());

    return nfa;
}

aotrc::fa::NFA aotrc::fa::nfa_builders::singleChar(char c) {
    aotrc::fa::NFA nfa;
    auto start = nfa.addState();
    auto end = nfa.addState();
    nfa.addEdge(start, end, Edge { Range(c) });

    return nfa;
}

aotrc::fa::NFA aotrc::fa::nfa_builders::literal(const std::string &literal) {
    NFA nfa;
    auto previousState = nfa.addState();
    for (const auto &c : literal) {
        auto nextState = nfa.addState();
        nfa.addEdge(previousState, nextState, Edge { Range(c) });
        previousState = nextState;
    }

    return nfa;
}

aotrc::fa::NFA aotrc::fa::nfa_builders::characterClass(const std::vector<Range> &ranges) {
    NFA cc;
    auto start = cc.addState();
    auto end = cc.addState();

    Edge edge;
    for (const auto &range : ranges) {
        edge.addRange(range);
    }

    cc.addEdge(start, end, std::move(edge));

    return cc;
}

static aotrc::fa::NFA concat_helper(aotrc::fa::NFA &&left, aotrc::fa::NFA &&right, unsigned int leftStartState, bool startEpsilon) {
    // For each state in right, add one to left, except for the first state
    std::unordered_map<unsigned int, unsigned int> rightStateTranslations;
    if (startEpsilon) {
        // Add a state real quick that will be the new zero state
        auto newZero = left.addState();
        left.addEdge(leftStartState, newZero, aotrc::fa::Edge());
        rightStateTranslations[0] = newZero;
    } else {
        rightStateTranslations[0] = leftStartState;
    }

    // Translate the remaining states
    for (int i = 1; i < right.stateCount(); i++) {
        rightStateTranslations[i] = left.addState();
    }

    // For each transition in right, move it to left
    for (int rightState = 0; rightState < right.stateCount(); rightState++) {
        auto transitions = right[rightState];
        for (auto &transition : transitions) {
            // Translate the two states involved
            auto leftOriginState = rightStateTranslations[rightState];
            auto leftDestState = rightStateTranslations[transition.first];

            // Make a transition
            left.addEdge(leftOriginState, leftDestState, std::move(transition.second));
        }
    }

    // return left
    return left;
}

aotrc::fa::NFA aotrc::fa::nfa_builders::concat(NFA &&left, NFA &&right) {
    auto lastState = left.stopState();
    return concat_helper(std::move(left), std::move(right), lastState, false);
}

aotrc::fa::NFA aotrc::fa::nfa_builders::alternation(NFA &&left, NFA &&right) {
    NFA alter;
    auto startState = alter.addState();
    // Concat the left branch with alter, starting at 0
    alter = concat_helper(std::move(alter), std::move(left), startState, true);
    // Take note of the left end state
    auto leftFinalState = alter.stopState();

    // Append the right branch
    alter = concat_helper(std::move(alter), std::move(right), startState, true);
    auto rightFinalState = alter.stopState();

    // Now, add a new final state and branch from each of those stop states to this one
    auto newFinalState = alter.addState();
    alter.addEdge(leftFinalState, newFinalState, Edge());
    alter.addEdge(rightFinalState, newFinalState, Edge());

    return alter;
}

aotrc::fa::NFA aotrc::fa::nfa_builders::star(NFA &&nfa) {
    NFA star;
    // Add a start state
    auto start = star.addState();
    // epsilon concat in the state
    star = concat_helper(std::move(star), std::move(nfa), start, true);
    auto enclosedStart = start + 1; // This is the state that the internal nfa starts with
    auto enclosedEnd = star.stopState(); // this is the state the internal nfa ends with

    // add a new stop state
    auto finalState = star.addState();

    // Add edges
    star.addEdge(enclosedEnd, enclosedStart, Edge());
    star.addEdge(start, finalState, Edge());
    star.addEdge(enclosedEnd, finalState, Edge());

    return star;
}
