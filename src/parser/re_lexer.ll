
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

%option noyywrap nounput noinput batch

SPECIAL_CHAR "[" | "]" | "(" | ")" | "{" | "}" | "+" | "*" | "?" | "^" | "$" | "-" | "!" | "?: | "." | "|" \ "\\"

%%

%{
  aotrc::parser::location &loc = driver.get_loc();
  loc.step();
%}

"["     return aotrc::parser::RegexParser::make_OPEN_CC(*yytext, loc);
"]"     return aotrc::parser::RegexParser::make_CLOSE_CC(*yytext, loc);
"("     return aotrc::parser::RegexParser::make_OPEN_GROUP(*yytext, loc);
")"     return aotrc::parser::RegexParser::make_CLOSE_GROUP(*yytext, loc);
"{"     return aotrc::parser::RegexParser::make_OPEN_BRACE(*yytext, loc);
"}"     return aotrc::parser::RegexParser::make_CLOSE_BRACE(*yytext, loc);
"+"     return aotrc::parser::RegexParser::make_PLUS(*yytext, loc);
"*"     return aotrc::parser::RegexParser::make_STAR(*yytext, loc);
"?"     return aotrc::parser::RegexParser::make_QUESTION(*yytext, loc);
"^"     return aotrc::parser::RegexParser::make_CARET(*yytext, loc);
"$"     return aotrc::parser::RegexParser::make_DOLLAR(*yytext, loc);
"-"     return aotrc::parser::RegexParser::make_HYPH(*yytext, loc);
"!"     return aotrc::parser::RegexParser::make_EXCLAM(*yytext, loc);
"?:"    return aotrc::parser::RegexParser::make_NONCAP(*yytext, loc);
"."     return aotrc::parser::RegexParser::make_PERIOD(*yytext, loc);
"|"     return aotrc::parser::RegexParser::make_PIPE(*yytext, loc);
"\\n"   return aotrc::parser::RegexParser::make_CHARACTER('\n', loc);
"\\t"   return aotrc::parser::RegexParser::make_CHARACTER('\t', loc);

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
    current_buffer = yy_scan_string(str);
    yy_switch_to_buffer(current_buffer);
}

void cleanup_buffer() {
    yy_delete_buffer(current_buffer);
    current_buffer = NULL;
}
