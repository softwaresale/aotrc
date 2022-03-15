//
// Created by charlie on 3/14/22.
//

#include <gtest/gtest.h>
#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(LiteralStringTest, "abc")

DECLARE_FA_ACCEPT_CASE(LiteralStringTest, AcceptLiteral, "abc")

DECLARE_FA_REJECT_CASE(LiteralStringTest, RejectMultipleLiterals, "abcabc")

DECLARE_FA_REJECT_CASE(LiteralStringTest, RejectTotallyDifferent, "bbbbbbbbbbaaaaaaa")

DECLARE_FA_REJECT_CASE(LiteralStringTest, RejectEmptyString, "")
