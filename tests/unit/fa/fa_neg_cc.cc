//
// Created by charlie on 3/17/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(NegCharacterClass, "[^a-zA-M123]")

DECLARE_FA_ACCEPT_CASE(NegCharacterClass, AcceptOutOfRange, "X")

DECLARE_FA_ACCEPT_CASE(NegCharacterClass, AcceptDifferentDigit, "5")

DECLARE_FA_ACCEPT_CASE(NegCharacterClass, AcceptTotallyDifferent, "^")

DECLARE_FA_REJECT_CASE(NegCharacterClass, RejectLowerCase, "d")

DECLARE_FA_REJECT_CASE(NegCharacterClass, RejectUpperInRange, "B")

DECLARE_FA_REJECT_CASE(NegCharacterClass, RejectNumber, "3")
