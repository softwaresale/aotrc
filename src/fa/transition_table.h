//
// Created by charlie on 12/2/21.
//

#ifndef _TRANSITION_TABLE_H
#define _TRANSITION_TABLE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <sstream>
#include <optional>
#include "transition_edge.h"

namespace aotrc::fa {
    /**
     * A transition table represents an abstract graph. It holds a sparse transition matrix. States
     * within the graph are unsigned integers, which correspond to rows in the matrix.
     */
    class TransitionTable {
    public:
        TransitionTable()
        : groups(0)
        {}

        virtual ~TransitionTable() = default;

        /**
         * Adds a new state to the table
         * @return newly created state
         */
        unsigned int addState();

        /**
         * Adds an edge between two states within the table
         * @param source vertex that it originates from
         * @param dest destination vertex
         * @param edge edge to transition by
         * @return true if success
         */
        bool addEdge(unsigned int source, unsigned int dest, const Edge& edge);

        /**
         * Adds an edge between two states within the table
         * @param source vertex that it originates from
         * @param dest destination vertex
         * @param edge edge to transition by
         * @return true if success
         */
        bool addEdge(unsigned int source, unsigned int dest, Edge&& edge);

        /**
         * Adds a new state with the given id
         * @param stateId MatchFunctionState to add
         * @return id of new state
         */
        unsigned int addState(unsigned int stateId);

        /**
         * Get's all of the ranges defined by this transition table
         * @return set of ranges that are used to transition in this table
         */
        RangeSet language() const;

        /**
         * Rather than getting all of the ranges from a transition table, this function actually gets everything
         * single character in the language
         * @return A set of characters that are in the language of this transition table
         */
        std::unordered_set<unsigned char> languageChars() const;

        /**
         * Gets all of the states reachable from the starting state by moving with the given character
         * @param state state to start at
         * @param c single character to transition by
         * @return set of fa states that can be reached from the start state moving by the given char
         */
        std::unordered_set<unsigned int> move(unsigned int state, unsigned char c) const;
        std::unordered_set<unsigned int> move(const std::unordered_set<unsigned int> &state, unsigned char c) const;

        /**
         * Get's all of the states that can be transitions away from state given the range
         * @param state MatchFunctionState to start from
         * @param range range to move by
         * @return set of accessible states
         */
        std::unordered_set<unsigned int> move(unsigned int state, const Range &range) const;
        std::unordered_set<unsigned int> move(const std::unordered_set<unsigned int>& state, const Range &range) const;

        /**
         * Access the transitions for the given state
         * @param state state to access
         * @return collection of transitions for that state
         */
        std::unordered_map<unsigned int, Edge> &operator[](unsigned int state);

        inline bool hasState(unsigned int state) const {
            return state < this->transitions.size();
        }

        inline const std::vector<std::unordered_map<unsigned int, Edge>> &getTransitions() const {
            return this->transitions;
        }

        inline const std::unordered_map<unsigned int, Edge> &edgesForState(unsigned int state) const {
            return this->transitions[state];
        }

        inline unsigned int stateCount() const {
            return this->transitions.size();
        }

        inline bool isLeaf(unsigned int state) const {
            return this->transitions[state].empty();
        }

        virtual inline bool isAcceptState(unsigned int state) const = 0;

        /**
         * Find the shortest path between the given start and end state
         * @param start Starting point
         * @param end ending point
         * @param testChar character to use to get between the two
         * @return A list of edges to take
         */
        std::vector<unsigned int> pathNodes(unsigned int start, unsigned int end, char testChar) const;

        /**
         * Find the shortest path between the given endpoints taking only the character. Take the edges on
         * the path
         * @param start Starting point
         * @param end Ending point
         * @param testChar character to take along path
         * @return Sequence of edges between the two
         */
        std::vector<Edge> pathEdges(unsigned int start, unsigned int end, char testChar) const;

        /**
         * Get all tags coverered when traversing between the two states
         * @param start
         * @param end
         * @param testChar
         * @return
         */
        std::unordered_set<int> tagClosure(unsigned int start, unsigned int end, char testChar) const;

        std::unordered_set<int> tagClosure(const std::unordered_set<unsigned int> &startStates, const std::unordered_set<unsigned int> &endStates, char testChar) const;

        unsigned int getGroupCount() const {
            return this->groups;
        }

        void setGroupCount(unsigned int count) {
            this->groups = count;
        }

        void incrementGroupCount() {
            this->groups += 1;
        };

    protected:
        // Sparse matrix of transitions
        std::vector<std::unordered_map<unsigned int, Edge>> transitions;
        unsigned int groups;
    };

    std::ostream &operator<<(std::ostream &os, const TransitionTable &table);
}

#endif //_TRANSITION_TABLE_H
