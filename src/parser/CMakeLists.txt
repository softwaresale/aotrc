
antlr4_generate(
        PCRE
        ${CMAKE_CURRENT_SOURCE_DIR}/PCRE.g4
        BOTH
        FALSE
        TRUE
        "aotrc::parser"
)

include_directories(${PROJECT_BINARY_DIR} ${ANTLR4_INCLUDE_DIR} ${ANTLR4_INCLUDE_DIR_PCRE})

add_library(aotrc_parser
        regex_parser.cc regex_parser.h
        ${ANTLR4_SRC_FILES_PCRE}
        regex_to_nfa.cc regex_to_nfa.h)

add_dependencies(aotrc_parser antlr4_shared)
target_link_libraries(aotrc_parser PUBLIC antlr4_shared)
target_link_libraries(aotrc_parser PRIVATE aotrc_fa)
