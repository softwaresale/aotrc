//
// Created by charlie on 3/15/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(ComboOneTest, "ab((de)|(fg))*hi");

DECLARE_FA_ACCEPT_CASE(ComboOneTest, AcceptLiteral, "abhi")

DECLARE_FA_ACCEPT_CASE(ComboOneTest, AcceptSomeDes, "abdedehi")

DECLARE_FA_ACCEPT_CASE(ComboOneTest, AcceptSomeFgs, "abfgfgfgfghi")

DECLARE_FA_ACCEPT_CASE(ComboOneTest, AcceptFgsAndDes, "abdefghi")

DECLARE_FA_REJECT_CASE(ComboOneTest, RejectMissingBeginning, "dedehi")

DECLARE_FA_REJECT_CASE(ComboOneTest, RejectMissingEnd, "abfgfg")

DECLARE_FA_REJECT_CASE(ComboOneTest, RejectBadMiddle, "abzzhi")

DECLARE_FA_REJECT_CASE(ComboOneTest, RejectBadMiddleCombo, "abdghi")