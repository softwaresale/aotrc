//
// Created by charlie on 3/14/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(PlusLiteralTest, "(?:abc)+")

DECLARE_FA_REJECT_CASE(PlusLiteralTest, RejectEmptyString, "")
DECLARE_FA_ACCEPT_CASE(PlusLiteralTest, AcceptLiteral, "abc");
DECLARE_FA_ACCEPT_CASE(PlusLiteralTest, AcceptMultipleLiteral, "abcabc");
DECLARE_FA_REJECT_CASE(PlusLiteralTest, RejectIncomplete, "abcab");
