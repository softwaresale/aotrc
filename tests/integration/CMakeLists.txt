
# Enable testing for this directory
include(GoogleTest)
enable_testing()

# Compile the testing regexes
set(AOTRC_EXE ${CMAKE_BINARY_DIR}/src/aotrc)
# set(AOTRC_TESTING_REGEXES_LIB ${CMAKE_BINARY_DIR}/tests/testing_regexes.a)
# set(AOTRC_TESTING_REGEXES_HEADER ${CMAKE_BINARY_DIR}/test/testing_regexes.h)
set(AOTRC_TESTING_REGEXES_LIB ${CMAKE_CURRENT_BINARY_DIR}/testing_regexes.a)
set(AOTRC_TESTING_REGEXES_HEADER ${CMAKE_CURRENT_BINARY_DIR}/testing_regexes.h)
add_custom_command(OUTPUT ${AOTRC_TESTING_REGEXES_LIB} ${AOTRC_TESTING_REGEXES_HEADER}
        COMMAND ${AOTRC_EXE} -t static ${CMAKE_CURRENT_SOURCE_DIR}/testing_regexes.yml
        COMMENT "Building testing regexes via aotrc"
        DEPENDS aotrc
        VERBATIM)
add_custom_target(libtesting_regexes_target DEPENDS ${AOTRC_TESTING_REGEXES_LIB} ${AOTRC_TESTING_REGEXES_HEADER})
add_library(libtesting_regexes STATIC IMPORTED GLOBAL)
add_dependencies(libtesting_regexes libtesting_regexes_target)
set_target_properties(libtesting_regexes
        PROPERTIES
        IMPORTED_LOCATION ${AOTRC_TESTING_REGEXES_LIB}
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})

# Build tests
add_library(int_base_tests
        int_base_test.cc int_base_test.h)
target_link_libraries(int_base_tests PUBLIC gtest_main libtesting_regexes)

function(define_int_test NAME)
    add_executable(${NAME} ${NAME}.cc)
    target_link_libraries(${NAME} PUBLIC int_base_tests)
    gtest_discover_tests(${NAME})
endfunction()

define_int_test(test_cc)
define_int_test(test_neg_cc)
define_int_test(test_plus)
define_int_test(test_star)
define_int_test(test_literal)
define_int_test(test_search)
define_int_test(test_alter)
