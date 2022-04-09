//
// Created by charlie on 3/26/22.
//

#include "int_base_test.h"

DECLARE_INT_TEST(CCTest, cc_full_match)

DECLARE_INT_ACCEPT_TEST(CCTest, AcceptLowerCaseString, "abcdefa")
DECLARE_INT_ACCEPT_TEST(CCTest, AcceptUpperCaseString, "WETISUDOG")

DECLARE_INT_ACCEPT_TEST(CCTest, AcceptNumber, "15235234")

DECLARE_INT_ACCEPT_TEST(CCTest, AcceptUnderscores, "______________")

DECLARE_INT_REJECT_TEST(CCTest, RejectSpecialChar, "%")

DECLARE_INT_TEST(CCSubmatchTest, cc_sub_match)

DECLARE_INT_ACCEPT_TEST(CCSubmatchTest, AcceptLowerCaseString, "))_____abcdefa_()*")
DECLARE_INT_ACCEPT_TEST(CCSubmatchTest, AcceptUpperAndNumberString, "))____3ASDF_()*")
