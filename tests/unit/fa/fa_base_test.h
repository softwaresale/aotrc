//
// Created by charlie on 3/14/22.
//

#ifndef ATORC_FA_BASE_TEST_H
#define ATORC_FA_BASE_TEST_H

#include <gtest/gtest.h>
#include "src/fa/nfa.h"
#include "src/parser/regex_parser.h"
#include "src/fa/dfa.h"

class NfaBaseTest : public ::testing::Test {
protected:
    explicit NfaBaseTest(std::string pattern)
    : pattern(std::move(pattern))
    { }

    void SetUp() override {
        this->nfa = aotrc::parser::parse_regex(this->pattern);
    }

    virtual inline bool accepts(const std::string &subject) {
        return this->nfa.simulate(subject);
    }

    virtual inline bool rejects(const std::string &subject) {
        return !this->nfa.simulate(subject);
    }

    std::string pattern;
    aotrc::fa::NFA nfa;
};

class DfaBaseTest : public NfaBaseTest {
protected:
    explicit DfaBaseTest(std::string pattern)
    : NfaBaseTest(std::move(pattern))
    { }

    void SetUp() override {
        // Create the NFA
        NfaBaseTest::SetUp();

        this->dfa = std::move(aotrc::fa::DFA(this->nfa));
    }

    bool accepts(const std::string &subject) override {
        return this->dfa.simulate(subject);
    }

    bool rejects(const std::string &subject) override {
        return !this->dfa.simulate(subject);
    }

    aotrc::fa::DFA dfa;
};

#define DECLARE_TEST_CLASS(ClassName, pattern) \
    class ClassName : public NfaBaseTest {     \
    public:                                    \
        ClassName() : NfaBaseTest(pattern) {}  \
    };

#define DECLARE_FA_TEST_CLASS(ClassName, pattern)  \
    class Dfa##ClassName : public DfaBaseTest {    \
    public:                                        \
        Dfa##ClassName() : DfaBaseTest(pattern) {} \
    };                                             \
    class Nfa##ClassName : public NfaBaseTest {    \
    public:                                        \
        Nfa##ClassName() : NfaBaseTest(pattern) {} \
    };

#define DECLARE_ACCEPT_CASE(ClassName, TestName, subject) \
    TEST_F(ClassName, TestName) {                         \
        ASSERT_TRUE(accepts(subject));                    \
    }

#define DECLARE_REJECT_CASE(ClassName, TestName, subject) \
    TEST_F(ClassName, TestName) {                         \
        ASSERT_TRUE(rejects(subject));                    \
    }

#define DECLARE_FA_ACCEPT_CASE(ClassName, TestName, subject) \
    DECLARE_ACCEPT_CASE(Nfa##ClassName, TestName, subject)   \
    DECLARE_ACCEPT_CASE(Dfa##ClassName, TestName, subject)

#define DECLARE_FA_REJECT_CASE(ClassName, TestName, subject) \
    DECLARE_REJECT_CASE(Nfa##ClassName, TestName, subject)   \
    DECLARE_REJECT_CASE(Dfa##ClassName, TestName, subject)

#endif //ATORC_FA_BASE_TEST_H
