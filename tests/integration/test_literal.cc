//
// Created by charlie on 3/26/22.
//

#include "int_base_test.h"

DECLARE_INT_TEST(LiteralTest, literal_full_match)

DECLARE_INT_ACCEPT_TEST(LiteralTest, AcceptLiteral, "abcd")
DECLARE_INT_REJECT_TEST(LiteralTest, RejectIncomplete, "abc")
DECLARE_INT_REJECT_TEST(LiteralTest, RejectDifferent, "abce")
DECLARE_INT_REJECT_TEST(LiteralTest, RejectEmpty, "")
