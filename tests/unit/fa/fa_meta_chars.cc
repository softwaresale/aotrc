//
// Created by charlie on 3/18/22.
//

#include "fa_base_test.h"

DECLARE_FA_TEST_CLASS(WordCharsTest, "\\w");

DECLARE_FA_ACCEPT_CASE(WordCharsTest, AcceptLowerCaseLetter, "a")
DECLARE_FA_ACCEPT_CASE(WordCharsTest, AcceptUpperCaseLetter, "G")

DECLARE_FA_ACCEPT_CASE(WordCharsTest, AcceptDigitLetter, "4")

DECLARE_FA_ACCEPT_CASE(WordCharsTest, AcceptUnderscore, "_")

DECLARE_FA_REJECT_CASE(WordCharsTest, AnythingElse, " ")

/* New Test */

DECLARE_FA_TEST_CLASS(NotWordCharsTest, "\\W")

DECLARE_FA_REJECT_CASE(NotWordCharsTest, RejectLowerCaseLetter, "a")
DECLARE_FA_REJECT_CASE(NotWordCharsTest, RejectUpperCaseLetter, "G")

DECLARE_FA_REJECT_CASE(NotWordCharsTest, RejectDigitLetter, "4")

DECLARE_FA_REJECT_CASE(NotWordCharsTest, RejectUnderscore, "_")

DECLARE_FA_ACCEPT_CASE(NotWordCharsTest, AcceptAnythingElse, "%")

/* New Test */

DECLARE_FA_TEST_CLASS(DigitTest, "\\d")

DECLARE_FA_ACCEPT_CASE(DigitTest, AcceptSingleDigit, "1")

DECLARE_FA_REJECT_CASE(DigitTest, RejectLowerLetter, "a")

DECLARE_FA_REJECT_CASE(DigitTest, RejectUpperLetter, "A")

DECLARE_FA_REJECT_CASE(DigitTest, RejectElse, "%")

/* New Test */

DECLARE_FA_TEST_CLASS(NotDigitTest, "\\D")

DECLARE_FA_REJECT_CASE(NotDigitTest, RejectSingleDigit, "1")

DECLARE_FA_ACCEPT_CASE(NotDigitTest, AcceptLowerLetter, "a")

DECLARE_FA_ACCEPT_CASE(NotDigitTest, AcceptUpperLetter, "A")

DECLARE_FA_ACCEPT_CASE(NotDigitTest, AcceptElse, "%")

/* New Test */

DECLARE_FA_TEST_CLASS(WhiteSpaceTest, "\\s")

DECLARE_FA_ACCEPT_CASE(WhiteSpaceTest, AcceptSpace, " ")

DECLARE_FA_ACCEPT_CASE(WhiteSpaceTest, AcceptNewline, "\n")

DECLARE_FA_ACCEPT_CASE(WhiteSpaceTest, AcceptTab, "\t")

DECLARE_FA_ACCEPT_CASE(WhiteSpaceTest, AcceptCarriageReturn, "\r")

DECLARE_FA_REJECT_CASE(WhiteSpaceTest, RejectLetter, "a")

/* New Test */

DECLARE_FA_TEST_CLASS(NotWhiteSpaceTest, "\\S")

DECLARE_FA_REJECT_CASE(NotWhiteSpaceTest, RejectSpace, " ")

DECLARE_FA_REJECT_CASE(NotWhiteSpaceTest, RejectNewline, "\n")

DECLARE_FA_REJECT_CASE(NotWhiteSpaceTest, RejectTab, "\t")

DECLARE_FA_REJECT_CASE(NotWhiteSpaceTest, RejectCarriageReturn, "\r")

DECLARE_FA_ACCEPT_CASE(NotWhiteSpaceTest, AcceptLetter, "a")
