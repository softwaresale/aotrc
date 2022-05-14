
# Enable testing for this directory
include(GoogleTest)
enable_testing()

add_executable(jit_test_fullmatch
        jit_test_fullmatch.cc)
target_link_libraries(jit_test_fullmatch PRIVATE gtest_main aotrc_jit)
gtest_discover_tests(jit_test_fullmatch)