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

/* Other character class test */
DECLARE_FA_TEST_CLASS(CharacterClassTest2, "[abcABC123]")

DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, AcceptLower_a, "a")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, AcceptLower_b, "b")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, AcceptLower_c, "c")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, AcceptUpper_a, "A")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, AcceptUpper_b, "B")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, AcceptUpper_c, "C")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, Accept_1, "1")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, Accept_2, "2")
DECLARE_FA_ACCEPT_CASE(CharacterClassTest2, Accept_3, "3")
