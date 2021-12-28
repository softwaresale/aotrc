//
// Created by charlie on 12/28/21.
//

#include <gtest/gtest.h>

#include <testing_regexes.h>

TEST(CorrectMatchRegexes, cc_is_correct) {
    const char *pass1 = "abcfweionf";
    const char *pass2 = "ASDIFJOWEOIFJ";
    const char *pass3 = "093729083_akjdfjklsdjf_ADJFIO";
    const char *pass4 = "*&#^abcef09#*&^$";
    const char *fail1 = "";
    const char *fail2 = "#&^$";

    EXPECT_TRUE(match_cc(pass1, strlen(pass1)));
    EXPECT_TRUE(match_cc(pass2, strlen(pass2)));
    EXPECT_TRUE(match_cc(pass3, strlen(pass3)));
    EXPECT_TRUE(match_cc(pass4, strlen(pass4)));
    EXPECT_FALSE(match_cc(fail1, strlen(fail1)));
    EXPECT_FALSE(match_cc(fail2, strlen(fail2)));
}

TEST(CorrectMatchRegexes, plus_is_correct) {
    const char *pass1 = "aaaaaa";
    const char *pass2 = "a";
    const char *pass3 = "aaaab";
    const char *fail1 = "";
    const char *fail2 = "bb";

    EXPECT_TRUE(match_plus(pass1, strlen(pass1)));
    EXPECT_TRUE(match_plus(pass2, strlen(pass2)));
    EXPECT_TRUE(match_plus(pass3, strlen(pass3)));
    EXPECT_FALSE(match_plus(fail1, strlen(fail1)));
    EXPECT_FALSE(match_plus(fail2, strlen(fail2)));
}

TEST(CorrectMatchRegexes, star_is_correct) {
    const char *pass1 = "aaaaaa";
    const char *pass2 = "a";
    const char *pass3 = "";
    const char *pass4 = "aaaab";
    const char *fail1 = "bb";

    EXPECT_TRUE(match_star(pass1, strlen(pass1)));
    EXPECT_TRUE(match_star(pass2, strlen(pass2)));
    EXPECT_TRUE(match_star(pass3, strlen(pass3)));
    EXPECT_TRUE(match_star(pass4, strlen(pass4)));
    EXPECT_FALSE(match_star(fail1, strlen(fail1)));
}

TEST(CorrectMatchRegexes, literal_is_correct) {
    const char *pass1 = "abcd";
    const char *pass2 = "abcdabcd";
    const char *fail1 = "aaaab";

    EXPECT_TRUE(match_literal(pass1, strlen(pass1)));
    EXPECT_TRUE(match_literal(pass2, strlen(pass2)));
    EXPECT_FALSE(match_literal(fail1, strlen(fail1)));
}
