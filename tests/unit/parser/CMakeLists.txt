
enable_testing()
include(GoogleTest)

function(define_parser_test NAME FILES)
    add_executable(${NAME} ${FILES})
    target_link_libraries(${NAME} PUBLIC gtest_main aotrc_parser aotrc_fa)
    gtest_discover_tests(${NAME})
endfunction()

define_parser_test(parse_literal parse_literal.cc)
define_parser_test(parse_quants parse_quants.cc)
define_parser_test(parse_specials parse_specials.cc)
define_parser_test(parse_numbers parse_numbers.cc)