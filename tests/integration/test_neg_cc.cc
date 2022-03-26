//
// Created by charlie on 3/26/22.
//

#include "int_base_test.h"

DECLARE_INT_TEST(NegCCTest, neg_cc_full_match);

DECLARE_INT_ACCEPT_TEST(NegCCTest, AcceptUpperCaseString, "ASDFEWF")
DECLARE_INT_ACCEPT_TEST(NegCCTest, AcceptNumbers, "3234252")
DECLARE_INT_ACCEPT_TEST(NegCCTest, AcceptWhiteSpaceAndSpecialChars, "   @*&^$&@  ")

DECLARE_INT_REJECT_TEST(NegCCTest, RejectLowerCaseString, "asdfiowefjo")