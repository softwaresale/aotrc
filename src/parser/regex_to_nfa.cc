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
        } else {
            // unsupported, so just don't quantify
            std::cerr << "WARNING: supported quantifier type: " << ctx->quantifier()->getText() << std::endl;
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
    } else {
        // Otherwise, this feature is not supported, so do nothing
        std::cerr << "WARNING: supported atom type: " << ctx->getText() << std::endl;
        builtNFA = fa::nfa_builders::epsilon();
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
