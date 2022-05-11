//
// Created by charlie on 3/26/22.
//

#include "int_base_test.h"

DECLARE_INT_TEST(StarTest, star_full_match)

DECLARE_INT_ACCEPT_TEST(StarTest, AcceptEmptyString, "")
DECLARE_INT_ACCEPT_TEST(StarTest, AcceptAs, "aaaaaaaa")

DECLARE_INT_REJECT_TEST(StarTest, RejectBs, "bbbbb")
DECLARE_INT_REJECT_TEST(StarTest, RejectAsThenB, "aaaaaaaaaaabbbbb")

#if 0
DECLARE_INT_TEST(StarSubmatchTest, star_sub_match)

DECLARE_INT_ACCEPT_TEST(StarSubmatchTest, BasicallyAlwaysAccept, "bb")
#endif