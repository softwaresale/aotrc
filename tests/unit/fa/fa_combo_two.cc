//
// Created by charlie on 3/15/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(ComboTwoTest, "[a-zA-Z_][a-zA-Z0-9_]* = [0-9]+")

DECLARE_FA_ACCEPT_CASE(ComboTwoTest, AcceptVarDec, "abc = 12")

DECLARE_FA_ACCEPT_CASE(ComboTwoTest, AcceptVarDec2, "AbCD0_12d = 1529861961827391")

DECLARE_FA_ACCEPT_CASE(ComboTwoTest, AcceptVarDec3, "_123_abc_ABCDEFEF = 092397520")

DECLARE_FA_REJECT_CASE(ComboTwoTest, RejectBadVar, "123abc_def_ABC = 123")

DECLARE_FA_REJECT_CASE(ComboTwoTest, RejectNoEquals, "abc   123")

DECLARE_FA_REJECT_CASE(ComboTwoTest, RejectMissingNum, "abc_def_ABC_123 = ")
