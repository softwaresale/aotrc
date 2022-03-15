
set(ANTLR4_JAR_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/antlr/antlr-4.9.3-complete.jar)

antlr4_generate(
        PCRE
        ${CMAKE_CURRENT_SOURCE_DIR}/PCRE.g4
        BOTH
        FALSE
        TRUE
        "aotrc::parser"
)

# antlr_target(PCREGrammar PCRE.g4 LEXER PARSER VISITOR
#         PACKAGE "aotrc::parser")
# include_directories(${ANTLR_PCREGrammar_OUTPUT_DIR})

include_directories(${PROJECT_BINARY_DIR} ${ANTLR4_INCLUDE_DIR} ${ANTLR4_INCLUDE_DIR_PCRE})

add_library(aotrc_parser
        regex_parser.cc regex_parser.h
        ${ANTLR_PCREGrammar_CXX_OUTPUTS}
        regex_to_nfa.cc regex_to_nfa.h)

add_dependencies(aotrc_parser antlr4_static)
target_link_libraries(aotrc_parser PUBLIC antlr4_static)
target_link_libraries(aotrc_parser PRIVATE aotrc_fa)
