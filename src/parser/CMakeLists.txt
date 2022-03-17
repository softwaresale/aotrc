
find_package(BISON)
find_package(FLEX)

BISON_TARGET(re_grammar re_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/re_grammar.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/re_grammar.tab.hh)
FLEX_TARGET(re_scanner re_lexer.ll ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/re_scanner.yy.hh)
ADD_FLEX_BISON_DEPENDENCY(re_scanner re_grammar)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(aotrc_parser
        regex_parser.cc regex_parser.h
        re_parser_driver.cc re_parser_driver.cc
        ${BISON_re_grammar_OUTPUTS}
        ${FLEX_re_scanner_OUTPUTS})
target_link_libraries(aotrc_parser PUBLIC aotrc_fa)

