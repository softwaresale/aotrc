
// Use a certain version of bison
%require "3.5.1"
// Explicitly use this variety of skeleton
%skeleton "lalr1.cc"

// Use variant api
%define api.token.constructor
%define api.value.type variant
%define parse.assert

// Change the name of the generated class to aotrc::parser::RegexParser
%define api.namespace {aotrc::parser}
%define api.parser.class {RegexParser}

%code requires {
    // Header code
    #include <iostream>
    #include <string>
    #include <vector>
    #include <functional>
    #include "src/fa/nfa.h"
    #include "src/fa/nfa_builders.h"

    // Pre-declare driver class
    namespace aotrc::parser {
        class RegexParserDriver;
    }
}

// TODO Give parse function a parameter to the driver (this is also passed to yylex)
%param { aotrc::parser::RegexParserDriver& driver }
%parse-param { aotrc::fa::NFA *resulting_nfa }

%locations
// Get more detailed location information
//%verbose
//%define parse.trace
//%define parse.error detailed
//%define parse.lac full

%code {
    #include <cmath>
    #include "re_parser_driver.h"
}

// TODO start defining tokens

%token YYEOF 0                    // backwards compatibility for bison v3.5.1 (builtin EOF is a new feature)
%token WORD_CHARS NOT_WORD_CHARS     // \w and \W
       NUMBER_CHARS NOT_NUMBER_CHARS // \d and \D
       WHITESPACE NOT_WHITESPACE     // \s and \S
       ;
%token <char> OPEN_CC CLOSE_CC    // Character classes
       OPEN_BRACE CLOSE_BRACE     // Start {}
       OPEN_GROUP CLOSE_GROUP     // Capture groups
       PLUS STAR QUESTION         // Quantifiers
       CARET DOLLAR HYPH          // multi-purpose operators for negation, ranges, and anchors
       EXCLAM COMMA               // General purpose stuff that might be used places
       PERIOD                     // . regex
       PIPE                       // | alternation operator
       NONCAP                     // Triggers a non-capture group
       DOT                        // Command character that represents anything
       ;
%token <char> ULETTER LLETTER CHARACTER;        // Represents a single character, upper or lower case
%token <unsigned int> DIGIT;

%type <std::vector<int>> digit_list;
%type <int> number;
%type <char> literal cc_literal;
%type <aotrc::fa::NFA> parse element atom character_class capture non_capture;
%type <std::vector<aotrc::fa::NFA>> alternation expr;
%type <std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)>> quantifier;
%type <std::vector<aotrc::fa::Range>> cc_atoms;
%type <aotrc::fa::Range> cc_atom;

%start parse

%%
parse
: alternation
{
    // Build the alternation
    auto builtNfa = aotrc::fa::nfa_builders::alternation(std::move($1));
    if (this->resulting_nfa)
        *this->resulting_nfa = std::move(builtNfa);
}
;

alternation
: expr
{

    // Convert the expr into a single NFA
    auto combined_expr = aotrc::fa::nfa_builders::concatMany(std::move($1));

    // Add the expression nfa to the back of the alternation
    $$.push_back(std::move(combined_expr));
}
| alternation PIPE expr
{
    // Convert the expr into a single NFA
    auto combined_expr = aotrc::fa::nfa_builders::concatMany(std::move($3));

    // Add expression
    $$ = $1;
    $$.push_back(std::move(combined_expr));
}
;

expr
: element
{
    $$.push_back($1);
}
| expr element
{
    $$ = $1;
    $$.push_back($2);
}
;

element
: atom
{
    // Leave the atom as is
    $$ = std::move($1);
}
| atom quantifier
{
    // Quantify the atom with the quantifier function
    $$ = std::invoke($2, std::move($1));
}
;

quantifier
: PLUS
{
    $$ = aotrc::fa::nfa_builders::plus;
}
| STAR
{
    $$ = aotrc::fa::nfa_builders::star;
}
| QUESTION
{
    $$ = aotrc::fa::nfa_builders::questionMark;
}
| OPEN_BRACE number CLOSE_BRACE
{
    unsigned int quantifier = $2;
    std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)> quant_operator
        = [quantifier](aotrc::fa::NFA &&nfa) { return aotrc::fa::nfa_builders::numberBounded(std::move(nfa), quantifier, quantifier); };
    $$ = std::move(quant_operator);
}
| OPEN_BRACE number COMMA CLOSE_BRACE
{
    unsigned int quant = $2;
    std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)> quant_operator
        = [quant](aotrc::fa::NFA &&nfa) { return aotrc::fa::nfa_builders::numberUnbounded(std::move(nfa), quant); };
    $$ = std::move(quant_operator);
}
| OPEN_BRACE number COMMA number CLOSE_BRACE
{
    unsigned int lower = $2;
    unsigned int upper = $4;
    std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)> quant_operator
        = [lower, upper](aotrc::fa::NFA &&nfa) { return aotrc::fa::nfa_builders::numberBounded(std::move(nfa), lower, upper); };
    $$ = std::move(quant_operator);
}
;

number
: digit_list
{
    int exp = 0;
    int sum = 0;
    for (auto it = $1.rbegin(); it != $1.rend(); ++it) {
        sum += std::pow(10, exp++) * *it;
    }

    $$ = sum;
}
;

digit_list
: DIGIT
{
    $$.push_back($1);
}
| digit_list DIGIT
{
    $$ = $1;
    $$.push_back($2);
}
;

character_class
: OPEN_CC cc_atoms CLOSE_CC
{
    $$ = aotrc::fa::nfa_builders::characterClass($2, false);
}
| OPEN_CC CARET cc_atoms CLOSE_CC
{
    $$ = aotrc::fa::nfa_builders::characterClass($3, true);
}
;

cc_atoms
: cc_atom
{
    $$.push_back($1);
}
| cc_atoms cc_atom
{
    $$ = $1;
    $$.push_back($2);
}
;

cc_atom
: cc_literal
{
    $$ = aotrc::fa::Range($1);
}
| cc_literal HYPH cc_literal
{
    // TODO check for validity here
    $$ = aotrc::fa::Range($1, $3);
}
;

cc_literal
: ULETTER
{
    $$ = $1;
}
| LLETTER
{
    $$ = $1;
}
| DIGIT
{
    // NOTE this should only ever be one character long
    auto number_str = std::to_string($1);
    $$ = number_str[0];
}
| QUESTION
{
    $$ = $1;
}
| PLUS
{
    $$ = $1;
}
| STAR
{
    $$ = $1;
}
| PIPE
{
    $$ = $1;
}
| OPEN_GROUP
{
    $$ = $1;
}
| CLOSE_GROUP
{
    $$ = $1;
}
| CHARACTER
{
    $$ = $1;
}
;

literal
: ULETTER
{
    $$ = $1;
}
| LLETTER
{
    $$ = $1;
}
| CHARACTER
{
    $$ = $1;
}
| DIGIT
{
    auto number_str = std::to_string($1);
    $$ = number_str[0];
}
| HYPH
{
    $$ = $1;
}
| EXCLAM
{
    $$ = $1;
}
;

capture
: OPEN_GROUP alternation CLOSE_GROUP
{
    auto internalNFA = aotrc::fa::nfa_builders::alternation(std::move($2));
    $$ = aotrc::fa::nfa_builders::group(std::move(internalNFA), this->driver.getNextGroupId());
}
;

non_capture
: OPEN_GROUP NONCAP alternation CLOSE_GROUP
{
    $$ = aotrc::fa::nfa_builders::alternation(std::move($3));
}
;

atom
: literal
{
    $$ = aotrc::fa::nfa_builders::singleChar($1);
}
| character_class
{
    // Character class is already compiled into an NFA
    $$ = std::move($1);
}
| capture
{
    // Capture is already compiled into an NFA
    $$ = std::move($1);
}
| non_capture
{
    // non-capture is already compiled into an NFA
    $$ = std::move($1);
}
| DOT
{
    $$ = std::move(aotrc::fa::nfa_builders::dot());
}
| WORD_CHARS
{
    auto nfa = aotrc::fa::nfa_builders::characterClass({ {'a', 'z'}, {'A', 'Z'}, {'0', '9'}, {'_'} }, false);
    $$ = nfa;
}
| NOT_WORD_CHARS
{
    auto nfa = aotrc::fa::nfa_builders::characterClass({ {'a', 'z'}, {'A', 'Z'}, {'0', '9'}, {'_'} }, true);
    $$ = nfa;
}
| NUMBER_CHARS
{
    auto nfa = aotrc::fa::nfa_builders::characterClass({ {'0', '9'} }, false);
    $$ = nfa;
}
| NOT_NUMBER_CHARS
{
    auto nfa = aotrc::fa::nfa_builders::characterClass({ {'0', '9'} }, true);
    $$ = nfa;
}
| WHITESPACE
{
    auto nfa = aotrc::fa::nfa_builders::characterClass({ {' '}, {'\n'}, {'\t'}, {'\r'} }, false);
    $$ = nfa;
}
| NOT_WHITESPACE
{
    auto nfa = aotrc::fa::nfa_builders::characterClass({ {' '}, {'\n'}, {'\t'}, {'\r'} }, true);
    $$ = nfa;
}
;


%%

void aotrc::parser::RegexParser::error(aotrc::parser::location const& loc, std::string const& msg) {
    std::cerr << "At " << loc << ": " << msg << std::endl;
}
