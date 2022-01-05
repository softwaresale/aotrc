//
// Created by charlie on 12/3/21.
//

#ifndef _REGEX_TO_NFA_H
#define _REGEX_TO_NFA_H

#include <PCREBaseVisitor.h>

namespace aotrc::parser {
    /**
     * Visitor that recursively transforms a regex into NFAs
     */
    class RegexToNFA : public aotrc::parser::PCREBaseVisitor {
    public:
        antlrcpp::Any visitParse(PCREParser::ParseContext *ctx) override;
        antlrcpp::Any visitAlternation(PCREParser::AlternationContext *ctx) override;
        antlrcpp::Any visitExpr(PCREParser::ExprContext *ctx) override;
        antlrcpp::Any visitAtom(PCREParser::AtomContext *ctx) override;
        antlrcpp::Any visitElement(PCREParser::ElementContext *ctx) override;
        antlrcpp::Any visitCharacter_class(PCREParser::Character_classContext *ctx) override;
        antlrcpp::Any visitCc_atom(PCREParser::Cc_atomContext *ctx) override;
        antlrcpp::Any visitLiteral(PCREParser::LiteralContext *ctx) override;
        antlrcpp::Any visitNon_capture(PCREParser::Non_captureContext *ctx) override;
    };
}

#endif //_REGEX_TO_NFA_H
