//
// Created by charlie on 12/28/21.
//

#include <gtest/gtest.h>

#include <testing_regexes.h>

TEST(CorrectFullRegexes, cc_is_correct) {
    const char *pass1 = "abcfweionf";
    const char *pass2 = "ASDIFJOWEOIFJ";
    const char *pass3 = "093729083_akjdfjklsdjf_ADJFIO";
    const char *fail1 = "";
    const char *fail2 = "#&^$";

    EXPECT_TRUE(fullmatch_cc(pass1, strlen(pass1)));
    EXPECT_TRUE(fullmatch_cc(pass2, strlen(pass2)));
    EXPECT_TRUE(fullmatch_cc(pass3, strlen(pass3)));
    EXPECT_FALSE(fullmatch_cc(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_cc(fail2, strlen(fail2)));
}

TEST(CorrectFullRegexes, neg_cc_is_correct) {
    const char *pass1 = "ABCE";
    const char *pass2 = "1534";
    const char *pass3 = "&*$";
    const char *fail1 = "abc";
    const char *fail2 = "ffflsdfj";
    const char *fail3 = "abcdefghijklmnopqrstuvwxyz";

    EXPECT_TRUE(fullmatch_neg_cc(pass1, strlen(pass1)));
    EXPECT_TRUE(fullmatch_neg_cc(pass2, strlen(pass2)));
    EXPECT_TRUE(fullmatch_neg_cc(pass3, strlen(pass3)));
    EXPECT_FALSE(fullmatch_neg_cc(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_neg_cc(fail2, strlen(fail2)));
    EXPECT_FALSE(fullmatch_neg_cc(fail3, strlen(fail3)));
}

TEST(CorrectFullRegexes, plus_is_correct) {
    const char *pass1 = "aaaaaa";
    const char *pass2 = "a";
    const char *fail1 = "";
    const char *fail2 = "bb";
    const char *fail3 = "aaaab";

    EXPECT_TRUE(fullmatch_plus(pass1, strlen(pass1)));
    EXPECT_TRUE(fullmatch_plus(pass2, strlen(pass2)));
    EXPECT_FALSE(fullmatch_plus(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_plus(fail2, strlen(fail2)));
    EXPECT_FALSE(fullmatch_plus(fail3, strlen(fail3)));
}

TEST(CorrectFullRegexes, star_is_correct) {
    const char *pass1 = "aaaaaa";
    const char *pass2 = "a";
    const char *pass3 = "";
    const char *fail1 = "bb";
    const char *fail2 = "aaaab";

    EXPECT_TRUE(fullmatch_star(pass1, strlen(pass1)));
    EXPECT_TRUE(fullmatch_star(pass2, strlen(pass2)));
    EXPECT_TRUE(fullmatch_star(pass3, strlen(pass3)));
    EXPECT_FALSE(fullmatch_star(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_star(fail2, strlen(fail2)));
}

TEST(CorrectFullRegexes, literal_is_correct) {
    const char *pass1 = "abcd";
    const char *fail1 = "abcdabcd";
    const char *fail2 = "aaaab";

    EXPECT_TRUE(fullmatch_literal(pass1, strlen(pass1)));
    EXPECT_FALSE(fullmatch_literal(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_literal(fail2, strlen(fail2)));
}

TEST(CorrectFullRegexes, number_bounded_single_is_correct) {
    const char *pass1 = "abcabcabc";
    const char *fail1 = "abc";
    const char *fail2 = "abcabc";
    const char *fail3 = "ab";
    const char *fail4 = "abcabcabcabc";
    const char *fail5 = "aaaaaa";

    EXPECT_TRUE(fullmatch_bounded_single(pass1, strlen(pass1)));
    EXPECT_FALSE(fullmatch_bounded_single(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_bounded_single(fail2, strlen(fail2)));
    EXPECT_FALSE(fullmatch_bounded_single(fail3, strlen(fail3)));
    EXPECT_FALSE(fullmatch_bounded_single(fail4, strlen(fail4)));
    EXPECT_FALSE(fullmatch_bounded_single(fail5, strlen(fail5)));
}


TEST(CorrectFullRegexes, number_bounded_range_is_correct) {
    const char *pass1 = "aaa";
    const char *pass2 = "aaaa";
    const char *pass3 = "aaaaa";
    const char *fail1 = "a";
    const char *fail2 = "aa";
    const char *fail3 = "aaaaaa";

    EXPECT_TRUE(fullmatch_bounded_range(pass1, strlen(pass1)));
    EXPECT_TRUE(fullmatch_bounded_range(pass2, strlen(pass2)));
    EXPECT_TRUE(fullmatch_bounded_range(pass3, strlen(pass3)));
    EXPECT_FALSE(fullmatch_bounded_range(fail1, strlen(fail1)));
    EXPECT_FALSE(fullmatch_bounded_range(fail2, strlen(fail2)));
    EXPECT_FALSE(fullmatch_bounded_range(fail3, strlen(fail3)));
}
