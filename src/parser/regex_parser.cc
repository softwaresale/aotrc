//
// Created by charlie on 12/3/21.
//

#include "regex_parser.h"

#include <antlr4-runtime/antlr4-runtime.h>
#include <PCRELexer.h>
#include <PCREParser.h>
#include "regex_to_nfa.h"

aotrc::fa::NFA aotrc::parser::parse_regex(const std::string &pattern) {
    antlr4::ANTLRInputStream antlrInputStream(pattern);
    PCRELexer lexer(&antlrInputStream);
    antlr4::CommonTokenStream tokenStream(&lexer);
    PCREParser parser(&tokenStream);

    RegexToNFA regexToNFAListener;
    return regexToNFAListener.visitParse(parser.parse()).as<fa::NFA>();
}
