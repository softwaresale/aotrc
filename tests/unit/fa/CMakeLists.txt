
enable_testing()
include(GoogleTest)

# Test NFAs

add_library(fa_base_tests
        fa_base_test.cc fa_base_test.h)
target_link_libraries(fa_base_tests gtest_main aotrc_fa aotrc_parser)

function(define_fa_test NAME)
    add_executable(${NAME} ${NAME}.cc)
    target_link_libraries(${NAME} fa_base_tests gtest_main)
    gtest_discover_tests(${NAME})
endfunction()

# Literal nfa
define_fa_test(fa_literal)
define_fa_test(fa_literal_str)
define_fa_test(fa_star_single)
define_fa_test(fa_star_literal)
define_fa_test(fa_plus_single)
define_fa_test(fa_plus_literal)
define_fa_test(fa_alternation_single)
define_fa_test(fa_alternation_literal)
define_fa_test(fa_character_class)
define_fa_test(fa_combo_one)
define_fa_test(fa_combo_two)
