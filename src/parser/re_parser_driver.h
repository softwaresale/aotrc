
#ifndef _AOTRC_PCRE_PARSER_H
#define _AOTRC_PCRE_PARSER_H

#include "re_grammar.tab.hh"
#include "location.hh"

/**
* Define the lexer symbol
*/
#define YY_DECL \
    aotrc::parser::RegexParser::symbol_type yylex(aotrc::parser::RegexParserDriver &driver)

/*
 * These functions are defined in re_lexer.ll. These functions are responsible for
 * setting the flex buffer state
 */

/**
 * Loads a flex buffer with a pattern
 * @param str Pattern to scan
 */
void load_buffer(const char *str);

/**
 * Cleans up the buffer state used by flex
 */
void cleanup_buffer();

namespace aotrc::parser {

    class RegexParserDriver {
    public:
        RegexParserDriver()
        : groups(0)
        {}
        // TODO parse function

        aotrc::parser::location &get_loc() {
            return this->loc;
        }

        int getNextGroupId();

        aotrc::fa::NFA parse(const std::string &subject);

    private:
        void startScan(const std::string &pattern);
        void stopScan();

        // Reference to the location class
        aotrc::parser::location loc;

        // Group count
        int groups;
    };
}

YY_DECL;

#endif // _AOTRC_PCRE_PARSER_H
