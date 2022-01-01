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

        bool operator==(const Range &otherRange) const noexcept {
            return otherRange.upper == this->upper && otherRange.lower == this->lower;
        }

        char lower;
        char upper;
    };

    struct RangeHash {
        std::size_t operator()(const aotrc::fa::Range &range) const noexcept {
            std::stringstream ss;
            ss << range.lower << range.upper;
            std::hash<std::string> string_hash;
            return string_hash(ss.str());
        }
    };

    struct RangeEquals {
        constexpr bool operator()(const aotrc::fa::Range &leftRange, const aotrc::fa::Range &rightRange) const noexcept {
            return leftRange.lower == rightRange.lower && leftRange.upper == rightRange.upper;
        }
    };
    using RangeSet = std::unordered_set<Range, RangeHash, RangeEquals>;

    class Edge {
    public:
        Edge()
        : rangesOptimized(false) {  };

        Edge(std::initializer_list<Range> ranges)
        : ranges(ranges)
        , rangesOptimized(false) {
        }

        /**
         * Collapse any ranges with overlap in order to minimize the number of ranges involved
         */
        void optimizeRanges();

        void addRange(Range range) {
            this->ranges.push_back(range);
            this->rangesOptimized = false;
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

        void merge(Edge &&other) {
            for (auto &range : other.ranges) {
                this->ranges.push_back(range);
            }
            this->rangesOptimized = false;
        }

        void merge(const Edge &other) {
            for (auto &range : other.ranges) {
                this->ranges.push_back(range);
            }
            this->rangesOptimized = false;
        }

        Edge complement();

    private:
        std::vector<Range> ranges;
        bool rangesOptimized;
    };


    class TransitionTable {
    public:
        TransitionTable() = default;
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

    protected:
        // Sparse matrix of transitions
        std::vector<std::unordered_map<unsigned int, Edge>> transitions;
    };

    std::ostream &operator<<(std::ostream &os, const TransitionTable &table);
}

#endif //_TRANSITION_TABLE_H
