//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_INT_BASE_TEST_H
#define AOTRC_INT_BASE_TEST_H

#include <gtest/gtest.h>
#include <testing_regexes.h>

class IntBaseTest : public ::testing::Test {
public:
    bool accepts(const std::string &str) {
        return regexFunc(str.c_str(), str.length());
    }

    bool rejects(const std::string &str) {
        return !regexFunc(str.c_str(), str.length());
    }

protected:
    explicit IntBaseTest(std::function<bool(const char *, long)> regexFunc)
    : regexFunc(std::move(regexFunc))
    {}

    void SetUp() override {
    }

    std::function<bool(const char *, long)> regexFunc;
};

#define DECLARE_INT_TEST(ClassName, function_name) \
class ClassName : public IntBaseTest {             \
public:                                            \
    ClassName()                                    \
    : IntBaseTest(function_name)                   \
    {} \
};

#define DECLARE_INT_ACCEPT_TEST(ClassName, FunctionName, TestString) \
TEST_F(ClassName, FunctionName) {                                   \
ASSERT_TRUE(accepts(TestString));                                    \
}

#define DECLARE_INT_REJECT_TEST(ClassName, FunctionName, TestString) \
TEST_F(ClassName, FunctionName) {                                   \
ASSERT_TRUE(rejects(TestString));                                \
}

#endif //AOTRC_INT_BASE_TEST_H
