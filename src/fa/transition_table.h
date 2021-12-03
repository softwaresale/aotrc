//
// Created by charlie on 12/2/21.
//

#ifndef _TRANSITION_TABLE_H
#define _TRANSITION_TABLE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>

namespace aotrc::fa {

    struct Range {
    public:
        Range(char lower, char upper)
        : lower(lower)
        , upper(upper) {
        }

        explicit Range(char single)
        : lower(single)
        , upper(single) {
        }

        char lower;
        char upper;
    };

    class Edge {
    public:
        Edge() = default;
        Edge(std::initializer_list<Range> ranges)
        : ranges(ranges) {
        }

        void addRange(Range range) {
            this->ranges.push_back(range);
        }

        const std::vector<Range> &getRanges() const {
            return ranges;
        }

        inline bool epsilon() const {
            return this->ranges.empty();
        }

        friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
            if (edge.epsilon()) {
                os << "$";
            } else {
                os << "{";
                for (const auto &range : edge.ranges) {
                    os << "[" << range.lower << "," << range.upper << "]";
                }
                os << "}";
            }

            return os;
        }

    private:
        std::vector<Range> ranges;
    };


    class TransitionTable {
    public:
        TransitionTable() = default;

        unsigned int addState();
        bool addEdge(unsigned int source, unsigned int dest, Edge&& edge);
        unsigned int addState(unsigned int stateId);
        friend std::ostream &operator<<(std::ostream &os, const TransitionTable &table);
        std::unordered_map<unsigned int, Edge> &operator[](unsigned int state);

        inline bool hasState(unsigned int state) {
            return state < this->transitions.size();
        }

        const std::vector<std::unordered_map<unsigned int, Edge>> &getTransitions() const {
            return this->transitions;
        }

        inline unsigned int stateCount() const {
            return this->transitions.size();
        }

    protected:
        // Sparse matrix of transitions
        std::vector<std::unordered_map<unsigned int, Edge>> transitions;
    };
}

#endif //_TRANSITION_TABLE_H
