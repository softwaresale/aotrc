//
// Created by charlie on 3/26/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(CharacterClass2, "[a-zm-tA-NM-Z0-9_]+")

DECLARE_FA_ACCEPT_CASE(CharacterClass2, AcceptLowerCase, "a")
