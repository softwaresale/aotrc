//
// Created by charlie on 3/14/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(LiteralTest, "a")

DECLARE_FA_ACCEPT_CASE(LiteralTest, AcceptsLiteral, "a")

DECLARE_FA_REJECT_CASE(LiteralTest, RejectsMultiple, "aa")

DECLARE_FA_REJECT_CASE(LiteralTest, RejectsNonCharacter, "b")

DECLARE_FA_REJECT_CASE(LiteralTest, RejectsLotsOfAsAndBs, "bbbbbbbbbbaaaaaaa")
