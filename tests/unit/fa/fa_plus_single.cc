//
// Created by charlie on 3/14/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(PlusSingleTest, "a+")

DECLARE_FA_REJECT_CASE(PlusSingleTest, RejectEmpty, "")
DECLARE_FA_ACCEPT_CASE(PlusSingleTest, AcceptSingle, "a")
DECLARE_FA_ACCEPT_CASE(PlusSingleTest, AcceptMultiple, "aaaa")

DECLARE_FA_REJECT_CASE(PlusSingleTest, RejectMulipleThenB, "aaaaab")

DECLARE_FA_REJECT_CASE(PlusSingleTest, RejectB, "baaa")