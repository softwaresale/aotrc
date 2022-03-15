//
// Created by charlie on 3/15/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(AlternationLiteral, "(?:abc)|(def)")

DECLARE_FA_ACCEPT_CASE(AlternationLiteral, AcceptFirst, "abc")
DECLARE_FA_ACCEPT_CASE(AlternationLiteral, AcceptSecond, "def")

DECLARE_FA_REJECT_CASE(AlternationLiteral, RejectBoth, "abcdef")

DECLARE_FA_REJECT_CASE(AlternationLiteral, RejectBothOther, "defabc")

DECLARE_FA_REJECT_CASE(AlternationLiteral, RejectNeither, "bcdaf")

DECLARE_FA_REJECT_CASE(AlternationLiteral, RejectEmpty, "")
