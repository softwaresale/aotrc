//
// Created by charlie on 5/11/22.
//

#include "int_base_test.h"

DECLARE_INT_TEST(AlterTest, alter_full_match)

DECLARE_INT_ACCEPT_TEST(AlterTest, AcceptLiteralOne, "abc")

DECLARE_INT_ACCEPT_TEST(AlterTest, AcceptLiteralTwo, "def")

DECLARE_INT_ACCEPT_TEST(AlterTest, AcceptUpperCaseString, "AGEFSDF")

DECLARE_INT_REJECT_TEST(AlterTest, RejectLowerCaseString, "defabc")

DECLARE_INT_REJECT_TEST(AlterTest, RejectIncompleteLiteral, "ab")

DECLARE_INT_REJECT_TEST(AlterTest, RejectWrongLiteral, "dea")

DECLARE_INT_REJECT_TEST(AlterTest, RejectRandomChars, "$@$")
