//
// Created by charlie on 12/3/21.
//

#include "regex_to_nfa.h"
#include "../fa/nfa.h"
#include "../fa/nfa_builders.h"

antlrcpp::Any aotrc::parser::RegexToNFA::visitElement(aotrc::parser::PCREParser::ElementContext *ctx) {
    // Build an NFA from the atom
    auto atomNfa = this->visitAtom(ctx->atom()).as<aotrc::fa::NFA>();

    // If there is a quantifier, add one
    if (ctx->quantifier()) {
        // Handle the start accordingly
        if (ctx->quantifier()->Star()) {
            atomNfa = aotrc::fa::nfa_builders::star(std::move(atomNfa));
        } else if (ctx->quantifier()->Plus()) {
            atomNfa = aotrc::fa::nfa_builders::plus(std::move(atomNfa));
        } else if (ctx->quantifier()->QuestionMark()) {
            atomNfa = aotrc::fa::nfa_builders::questionMark(std::move(atomNfa));
        } else if (ctx->quantifier()->OpenBrace()) {
            // Numbered quantifier
            if (ctx->quantifier()->Comma()) {
                // If there is one number, then it's unbounded. Otherwise, it's a range
                if (ctx->quantifier()->number().size() > 1) {
                    auto first = std::stoi(ctx->quantifier()->number(0)->getText());
                    auto second = std::stoi(ctx->quantifier()->number(1)->getText());
                    atomNfa = aotrc::fa::nfa_builders::numberBounded(std::move(atomNfa), first, second);
                } else {
                    auto first = std::stoi(ctx->quantifier()->number(0)->getText());
                    atomNfa = aotrc::fa::nfa_builders::numberUnbounded(std::move(atomNfa), first);
                }
            } else {
                // It's just a single repetition
                auto first = std::stoi(ctx->quantifier()->number(0)->getText());
                atomNfa = aotrc::fa::nfa_builders::numberBounded(std::move(atomNfa), first, first);
            }
        } else {
            // unsupported, so fail
            throw std::runtime_error("Unsupported quantifier type: " + ctx->quantifier()->getText());
        }
    }

    return atomNfa;
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitAtom(aotrc::parser::PCREParser::AtomContext *ctx) {
    antlrcpp::Any builtNFA;
    if (ctx->literal()) {
        auto literalText = this->visitLiteral(ctx->literal()).as<std::string>();
        // Build an NFA out of the literal
        builtNFA = aotrc::fa::nfa_builders::literal(literalText);
    } else if (ctx->capture()) {
        builtNFA = this->visitAlternation(ctx->capture()->alternation()).as<fa::NFA>();
    } else if (ctx->character_class()) {
        builtNFA = this->visitCharacter_class(ctx->character_class());
    } else if (ctx->non_capture()) {
        builtNFA = this->visitNon_capture(ctx->non_capture());
    } else {
        // Otherwise, this feature is not supported, so do nothing
        throw std::runtime_error("Unsupported atom type: " + ctx->getText());
    }

    return builtNFA;
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitExpr(aotrc::parser::PCREParser::ExprContext *ctx) {
    // Convert each element into an NFA
    std::vector<fa::NFA> nfas;
    for (const auto elementCtx : ctx->element()) {
        auto elementNFA = this->visitElement(elementCtx).as<fa::NFA>();
        nfas.push_back(std::move(elementNFA));
    }

    // Concatenate all the nfas together
    fa::NFA combinedNFA;
    for (auto nfa : nfas) {
        // Concat each one to the end of the combined one
        combinedNFA = aotrc::fa::nfa_builders::concat(std::move(combinedNFA), std::move(nfa));
    }

    return combinedNFA;
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitAlternation(aotrc::parser::PCREParser::AlternationContext *ctx) {
    // Set the first NFA
    fa::NFA combinedNFA = this->visitExpr(ctx->expr(0)).as<fa::NFA>();

    // if there are more, alternate them all
    if (ctx->expr().size() > 1) {
        for (unsigned int i = 1; i < ctx->expr().size(); i++) {
            // Build the NFA
            auto exprNFA = this->visitExpr(ctx->expr(i)).as<fa::NFA>();
            // alternate it with the original one
            combinedNFA = aotrc::fa::nfa_builders::alternation(std::move(combinedNFA), std::move(exprNFA));
        }
    }

    return combinedNFA;
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitParse(aotrc::parser::PCREParser::ParseContext *ctx) {
    return this->visitAlternation(ctx->alternation());
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitCharacter_class(aotrc::parser::PCREParser::Character_classContext *ctx) {
    // Figure out if we are working with a negated character class
    bool isNegated = ctx->Caret() != nullptr;

    // Get the ranges of the character class
    std::vector<fa::Range> ranges;
    for (const auto &atom : ctx->cc_atom()) {
        // Get a vector of ranges back from visiting an atom
        auto rangeVec = this->visitCc_atom(atom).as<std::vector<fa::Range>>();
        std::move(rangeVec.begin(),  rangeVec.end(), std::back_inserter(ranges));
    }

    // Build a cc
    return fa::nfa_builders::characterClass(ranges, isNegated);
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitCc_atom(aotrc::parser::PCREParser::Cc_atomContext *ctx) {
    // Get a range from a character class atom
    std::vector<fa::Range> ranges;
    if (ctx->Hyphen()) {
        // It's a range of two literals
        char first = ctx->cc_literal(0)->getText()[0];
        char second = ctx->cc_literal(1)->getText()[0];
        ranges.emplace_back(first, second);
    } else if (ctx->shared_atom()) {
        auto shared = ctx->shared_atom();
        if (shared->WordChar()) {
            // equivalent to a-zA-Z0-9_
            ranges.emplace_back('a', 'z');
            ranges.emplace_back('A', 'Z');
            ranges.emplace_back('0', '9');
            ranges.emplace_back('_');
        } else if (shared->DecimalDigit()) {
            ranges.emplace_back('0', '9');
        } else if (shared->WhiteSpace()) {
            ranges.emplace_back('\n');
            ranges.emplace_back('\t');
            ranges.emplace_back('\r');
            ranges.emplace_back(' ');
        } else {
            throw std::runtime_error("Unsupported shared atom: " + shared->getText());
        }
    } else if (!ctx->cc_literal().empty()) {
        char only;
        if (ctx->cc_literal(0)->shared_literal() && ctx->cc_literal(0)->shared_literal()->Quoted()) {
            // In the case of something quoted, remove the backslash
            only = ctx->cc_literal(0)->shared_literal()->getText()[1];
        } else {
            // Otherwise, get the content of the shared_literal
            only = ctx->cc_literal(0)->getText()[0];
        }
        ranges.emplace_back(only);
    } else {
        throw std::runtime_error("Error while handling character class atom '" + ctx->getText() + "'");
    }

    return ranges;
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitLiteral(aotrc::parser::PCREParser::LiteralContext *ctx) {
    char literal;
    if (ctx->shared_literal() && ctx->shared_literal()->Quoted()) {
        literal = ctx->shared_literal()->Quoted()->getText()[1];
    } else {
        literal = ctx->getText()[0];
    }

    return std::string(&literal, 1);
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitNon_capture(aotrc::parser::PCREParser::Non_captureContext *ctx) {
    return this->visitAlternation(ctx->alternation());
}
