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

aotrc::fa::NFA aotrc::fa::nfa_builders::characterClass(const std::vector<Range> &ranges, bool negated) {
    NFA cc;
    auto start = cc.addState();
    auto end = cc.addState();

    Edge edge;
    for (const auto &range : ranges) {
        edge.addRange(range);
    }

    // Optimize the edge's ranges (remove anything that's redundant)
    edge.optimizeRanges();

    // If negated range, then complement edge
    if (negated) {
        edge = edge.complement();
    }

    cc.addEdge(start, end, std::move(edge));

    return cc;
}

static aotrc::fa::NFA concat_helper(aotrc::fa::NFA &&left, aotrc::fa::NFA &&right, unsigned int leftStartState, bool startEpsilon) {

    // if either left or right are empty, then just return through
    if (left.stateCount() == 0 && right.stateCount() > 0) {
        return std::move(right);
    } else if (right.stateCount() == 0 && left.stateCount() > 0) {
        return std::move(left);
    } else if (left.stateCount() == 0 && right.startState() == 0) {
        return std::move(left);
    }

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
    for (unsigned int i = 1; i < right.stateCount(); i++) {
        rightStateTranslations[i] = left.addState();
    }

    // For each transition in right, move it to left
    for (unsigned int rightState = 0; rightState < right.stateCount(); rightState++) {
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
    return std::move(left);
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

aotrc::fa::NFA aotrc::fa::nfa_builders::plus(aotrc::fa::NFA &&nfa) {
    // Build a star from the original
    NFA copy = nfa;
    // Star
    auto starNFA = star(std::move(nfa));
    // Concat the copy with star
    return concat(std::move(copy), std::move(starNFA));
}

aotrc::fa::NFA aotrc::fa::nfa_builders::questionMark(aotrc::fa::NFA &&nfa) {
    // Question mark is a (r|e)
    auto left = epsilon();
    return alternation(std::move(left), std::move(nfa));
}

aotrc::fa::NFA aotrc::fa::nfa_builders::numberBounded(NFA &&nfa, unsigned int lower, unsigned int higher) {
    NFA parent = std::move(nfa);
    std::vector<NFA> repetitionBranches;
    for (unsigned int repetitions = lower; repetitions <= higher; repetitions++) {
        NFA repetitionBranch;
        for (unsigned int instance = 0; instance < repetitions; instance++) {
            // Make a copy and concat it with the other one
            NFA copy = parent;
            repetitionBranch = concat(std::move(repetitionBranch), std::move(copy));
        }
        repetitionBranches.push_back(std::move(repetitionBranch));
    }
    NFA quantified = std::move(repetitionBranches[0]);
    repetitionBranches.erase(repetitionBranches.begin());
    for (auto &branch : repetitionBranches) {
        quantified = alternation(std::move(quantified), std::move(branch));
    }

    return quantified;
}

aotrc::fa::NFA aotrc::fa::nfa_builders::numberUnbounded(aotrc::fa::NFA &&nfa, unsigned int lower) {
    NFA parent = std::move(nfa);
    NFA result = parent;
    for (unsigned int i = 0; i < lower - 2; i++) {
        NFA copy = parent;
        result = concat(std::move(result), std::move(copy));
    }

    NFA nfaStar = star(std::move(parent));
    result = concat(std::move(result), std::move(nfaStar));
    return result;
}
