
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
    // TODO include driver info
    #include "re_parser_driver.h"
}

// TODO start defining tokens

%token YYEOF 0                    // backwards compatibility for bison v3.5.1 (builtin EOF is a new feature)
%token OPEN_CC CLOSE_CC         // Character classes
       OPEN_BRACE CLOSE_BRACE   // Start {}
       OPEN_GROUP CLOSE_GROUP   // Capture groups
       PLUS STAR QUESTION       // Quantifiers
       CARET DOLLAR HYPH        // multi-purpose operators for negation, ranges, and anchors
       EXCLAM COMMA             // General purpose stuff that might be used places
       PERIOD                   // . regex
       PIPE                     // | alternation operator
       NONCAP                   // Triggers a non-capture group
       ;
%token <char> ULETTER LLETTER CHARACTER;        // Represents a single character, upper or lower case
%token <unsigned int> NUMBER DIGIT;

%type <char> literal;
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
| OPEN_BRACE NUMBER CLOSE_BRACE
{
    unsigned int quantifier = $2;
    std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)> quant_operator
        = [quantifier](aotrc::fa::NFA &&nfa) { return aotrc::fa::nfa_builders::numberBounded(std::move(nfa), quantifier, quantifier); };
    $$ = std::move(quant_operator);
}
| OPEN_BRACE NUMBER COMMA CLOSE_BRACE
{
    unsigned int quant = $2;
    std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)> quant_operator
        = [quant](aotrc::fa::NFA &&nfa) { return aotrc::fa::nfa_builders::numberUnbounded(std::move(nfa), quant); };
    $$ = std::move(quant_operator);
}
| OPEN_BRACE NUMBER COMMA NUMBER CLOSE_BRACE
{
    unsigned int lower = $2;
    unsigned int upper = $4;
    std::function<aotrc::fa::NFA(aotrc::fa::NFA&&)> quant_operator
        = [lower, upper](aotrc::fa::NFA &&nfa) { return aotrc::fa::nfa_builders::numberBounded(std::move(nfa), lower, upper); };
    $$ = std::move(quant_operator);
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
: literal
{
    $$ = aotrc::fa::Range($1);
}
| literal HYPH literal
{
    // TODO check for validity here
    $$ = aotrc::fa::Range($1, $3);
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
| NUMBER
{
    // NOTE this should only ever be one character long
    auto number_str = std::to_string($1);
    $$ = number_str[0];
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
;

capture
: OPEN_GROUP alternation CLOSE_GROUP
{
    $$ = aotrc::fa::nfa_builders::alternation(std::move($2));
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
;


%%

void aotrc::parser::RegexParser::error(aotrc::parser::location const& loc, std::string const& msg) {
    std::cerr << "At " << loc << ": " << msg << std::endl;
}
