//
// Created by charlie on 3/14/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(StarSingleTest, "a*")

DECLARE_FA_ACCEPT_CASE(StarSingleTest, AcceptEmptyString, "")

DECLARE_FA_ACCEPT_CASE(StarSingleTest, AcceptSingle, "a")

DECLARE_FA_ACCEPT_CASE(StarSingleTest, AcceptLots, "aaaaaaaaa")

DECLARE_FA_REJECT_CASE(StarSingleTest, RejectAsThenB, "aaaaaaaaaaab")

DECLARE_FA_REJECT_CASE(StarSingleTest, RejectBThenA, "bbbbbaaaaa")
