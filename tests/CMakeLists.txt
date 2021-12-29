
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Compile the testing regexes
set(AOTRC_EXE ${CMAKE_BINARY_DIR}/src/aotrc)
set(AOTRC_TESTING_REGEXES_LIB ${CMAKE_BINARY_DIR}/tests/libtesting_regexes.a)
set(AOTRC_TESTING_REGEXES_HEADER ${CMAKE_BINARY_DIR}/test/testing_regexes.h)
add_custom_command(OUTPUT ${AOTRC_TESTING_REGEXES_LIB} ${AOTRC_TESTING_REGEXES_HEADER}
        COMMAND ${AOTRC_EXE} -a ${CMAKE_CURRENT_SOURCE_DIR}/testing_regexes.yml
        COMMENT "Building testing regexes via aotrc"
        DEPENDS aotrc
        VERBATIM)
add_custom_target(libtesting_regexes_target DEPENDS ${AOTRC_TESTING_REGEXES_LIB} ${AOTRC_TESTING_REGEXES_HEADER})
add_library(libtesting_regexes STATIC IMPORTED GLOBAL)
add_dependencies(libtesting_regexes libtesting_regexes_target)
set_target_properties(libtesting_regexes
        PROPERTIES
        IMPORTED_LOCATION ${AOTRC_TESTING_REGEXES_LIB}
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/tests)

# define executables here
add_executable(correct_fullmatch_regexes_test integration/correct_fullmatch_regexes_tests.cc)
target_link_libraries(correct_fullmatch_regexes_test PRIVATE ${AOTRC_ALL_LIBS} libtesting_regexes gtest_main)

add_executable(correct_match_regexes_test integration/correct_match_regexes_test.cc)
target_link_libraries(correct_match_regexes_test PRIVATE ${AOTRC_ALL_LIBS} libtesting_regexes gtest_main)

include(GoogleTest)
gtest_discover_tests(correct_fullmatch_regexes_test)
gtest_discover_tests(correct_match_regexes_test)
