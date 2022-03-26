//
// Created by charlie on 3/26/22.
//

#include "int_base_test.h"

DECLARE_INT_TEST(PlusTest, plus_full_match)

DECLARE_INT_ACCEPT_TEST(PlusTest, AcceptLotsOfAs, "aaaaaaaaaaaaa")

DECLARE_INT_ACCEPT_TEST(PlusTest, AcceptAnA, "a")

DECLARE_INT_REJECT_TEST(PlusTest, RejectEmpty, "")

DECLARE_INT_REJECT_TEST(PlusTest, RejectAsThenB, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab")
DECLARE_INT_REJECT_TEST(PlusTest, RejectBs, "bbbbbb")
