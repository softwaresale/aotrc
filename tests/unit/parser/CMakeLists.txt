
enable_testing()
include(GoogleTest)

add_executable(parse_literal parse_literal.cc)
target_link_libraries(parse_literal PUBLIC gtest_main aotrc_parser aotrc_fa)
gtest_discover_tests(parse_literal)