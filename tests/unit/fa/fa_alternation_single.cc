//
// Created by charlie on 3/14/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(AlternationSingleTest, "a|b")

DECLARE_FA_ACCEPT_CASE(AlternationSingleTest, AcceptA, "a")
DECLARE_FA_ACCEPT_CASE(AlternationSingleTest, AcceptB, "b")
DECLARE_FA_REJECT_CASE(AlternationSingleTest, RejectAAndB, "ab")
DECLARE_FA_REJECT_CASE(AlternationSingleTest, RejectC, "c")
DECLARE_FA_REJECT_CASE(AlternationSingleTest, RejectEmpty, "")