//
// Created by charlie on 3/14/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(StarLiteralTest, "(?:abc)*")

DECLARE_FA_ACCEPT_CASE(StarLiteralTest, AcceptEmptyString, "")

DECLARE_FA_ACCEPT_CASE(StarLiteralTest, AcceptStringLiteral, "abc")

DECLARE_FA_ACCEPT_CASE(StarLiteralTest, AcceptMultipleLiteral, "abcabc")

DECLARE_FA_REJECT_CASE(StarLiteralTest, RejectSingleIncomplete, "ab")

DECLARE_FA_REJECT_CASE(StarLiteralTest, RejectMultipleIncomplete, "abcab")
