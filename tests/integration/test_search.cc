//
// Created by charlie on 4/18/22.
//

#include "int_base_test.h"

class SearchTest : public ::testing::Test {
public:
    SearchTest()
    : regexFunc(literal_search)
    {  }

protected:
    std::function<bool(const char *, long, long *, long *)> regexFunc;
};

TEST_F(SearchTest, CaptureWholeThing) {
    std::string subject("abcd");
    long start = 0, finish = 0;
    auto matches = std::invoke(this->regexFunc, subject.c_str(), subject.length(), &start, &finish);
    ASSERT_TRUE(matches);
    ASSERT_EQ(start, 0);
    ASSERT_EQ(finish, 4);
}

TEST_F(SearchTest, CorrectlyFindRegex) {
    std::string subject("DGSDFabcdAEGFS");
    // Should correctly find the substring
    long start = 0, finish = 0;
    auto matches = std::invoke(this->regexFunc, subject.c_str(), subject.length(), &start, &finish);
    ASSERT_TRUE(matches);
    ASSERT_EQ(start, 5);
    ASSERT_EQ(finish, 9);
}

TEST_F(SearchTest, FindAtBeginning) {
    std::string subject = "abcdEGSE";
    long start = 0, finish = 0;
    auto matches = std::invoke(this->regexFunc, subject.c_str(), subject.size(), &start, &finish);
    ASSERT_TRUE(matches);
    ASSERT_EQ(start, 0);
    ASSERT_EQ(finish, 4);
}

TEST_F(SearchTest, FindAtEnd) {
    std::string subject = "SEGSDFAabcd";
    long start = 0, finish = 0;
    auto matches = std::invoke(this->regexFunc, subject.c_str(), subject.size(), &start, &finish);
    ASSERT_TRUE(matches);
    ASSERT_EQ(start, 7);
    ASSERT_EQ(finish, 11);
}

