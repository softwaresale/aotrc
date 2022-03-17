
%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring> // strerror
#include <string>
#include "re_parser_driver.h"
#include "re_grammar.tab.hh"
#include "location.hh"

YY_BUFFER_STATE current_buffer;
void load_buffer(const char *str);
void cleanup_buffer();

/**
* TODO need to handle escaped metacharacters
*/

#define YY_USER_ACTION  loc.columns (yyleng);
%}

%option noyywrap nounput noinput batch debug

SPECIAL_CHAR "[" | "]" | "(" | ")" | "{" | "}" | "+" | "*" | "?" | "^" | "$" | "-" | "!" | "?: | "." | "|" \ "\\"

%%

%{
  aotrc::parser::location &loc = driver.get_loc();
  loc.step();
%}

"["     return aotrc::parser::RegexParser::make_OPEN_CC(loc);
"]"     return aotrc::parser::RegexParser::make_CLOSE_CC(loc);
"("     return aotrc::parser::RegexParser::make_OPEN_GROUP(loc);
")"     return aotrc::parser::RegexParser::make_CLOSE_GROUP(loc);
"{"     return aotrc::parser::RegexParser::make_OPEN_BRACE(loc);
"}"     return aotrc::parser::RegexParser::make_CLOSE_BRACE(loc);
"+"     return aotrc::parser::RegexParser::make_PLUS(loc);
"*"     return aotrc::parser::RegexParser::make_STAR(loc);
"?"     return aotrc::parser::RegexParser::make_QUESTION(loc);
"^"     return aotrc::parser::RegexParser::make_CARET(loc);
"$"     return aotrc::parser::RegexParser::make_DOLLAR(loc);
"-"     return aotrc::parser::RegexParser::make_HYPH(loc);
"!"     return aotrc::parser::RegexParser::make_EXCLAM(loc);
"?:"    return aotrc::parser::RegexParser::make_NONCAP(loc);
"."     return aotrc::parser::RegexParser::make_PERIOD(loc);
"|"     return aotrc::parser::RegexParser::make_PIPE(loc);

[a-z]   return aotrc::parser::RegexParser::make_LLETTER(*yytext, loc);
[A-Z]   return aotrc::parser::RegexParser::make_ULETTER(*yytext, loc);
[0-9]   return aotrc::parser::RegexParser::make_DIGIT(atoi(yytext), loc);
[0-9]+  return aotrc::parser::RegexParser::make_NUMBER(atoi(yytext), loc);
[_ ]    return aotrc::parser::RegexParser::make_CHARACTER(*yytext, loc);
[\n\t]+  throw aotrc::parser::RegexParser::syntax_error(loc, "Newlines and tabs cannot be in regex def");

.       return aotrc::parser::RegexParser::make_CHARACTER(*yytext, loc);

<<EOF>> return aotrc::parser::RegexParser::make_YYEOF(loc);

%%

void load_buffer(const char *str) {
    fprintf(stderr, "Flex: loading buffer\n");
    fprintf(stderr, "Flex: current buffer prior to loading: %p\n", YY_CURRENT_BUFFER);
    current_buffer = yy_scan_string(str);
    yy_switch_to_buffer(current_buffer);
    fprintf(stderr, "Flex: current buffer after loading: %p\n", YY_CURRENT_BUFFER);
}

void cleanup_buffer() {
    fprintf(stderr, "Flex: cleaning up buffer\n");
    yy_delete_buffer(current_buffer);
    current_buffer = NULL;
}
