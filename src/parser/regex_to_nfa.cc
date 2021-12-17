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
        } else {
            // unsupported, so just don't quantify
            // std::cerr << "WARNING: supported quantifier type: " << ctx->quantifier()->getText() << std::endl;
            throw std::runtime_error("Unsupported quantifier type: " + ctx->quantifier()->getText());
        }
    }

    return atomNfa;
}

antlrcpp::Any aotrc::parser::RegexToNFA::visitAtom(aotrc::parser::PCREParser::AtomContext *ctx) {
    antlrcpp::Any builtNFA;
    if (ctx->literal()) {
        auto literalText = ctx->literal()->getText();
        // Build an NFA out of the literal
        auto literalNFA = aotrc::fa::nfa_builders::literal(literalText);

        builtNFA = literalNFA;
    } else if (ctx->capture()) {
        builtNFA = this->visitAlternation(ctx->capture()->alternation()).as<fa::NFA>();
    } else if (ctx->character_class()) {
        builtNFA = this->visitCharacter_class(ctx->character_class());
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
        for (int i = 1; i < ctx->expr().size(); i++) {
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
    if (ctx->Caret()) {
        throw std::runtime_error("Negated character classes are not supported");
    }

    // Get the ranges of the character class
    std::vector<fa::Range> ranges;
    for (const auto &atom : ctx->cc_atom()) {
        // Get a vector of ranges back from visiting an atom
        auto rangeVec = this->visitCc_atom(atom).as<std::vector<fa::Range>>();
        std::move(rangeVec.begin(),  rangeVec.end(), std::back_inserter(ranges));
    }

    // Build a cc
    return fa::nfa_builders::characterClass(ranges);
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
    } else {
        char only = ctx->cc_literal(0)->getText()[0];
        ranges.emplace_back(only);
    }

    return ranges;
}
