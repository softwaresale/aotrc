//
// Created by charlie on 3/15/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(CharacterClassTest, "[a-zA-M123]")

DECLARE_FA_ACCEPT_CASE(CharacterClassTest, AcceptInRange, "e")

DECLARE_FA_ACCEPT_CASE(CharacterClassTest, AcceptAtom, "1")

DECLARE_FA_REJECT_CASE(CharacterClassTest, RejectOutOfRange, "P")

DECLARE_FA_REJECT_CASE(CharacterClassTest, RejectNonLiteral, "5")

DECLARE_FA_REJECT_CASE(CharacterClassTest, RejectMultiple, "aA3")
