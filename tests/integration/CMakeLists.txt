
# Enable testing for this directory
include(GoogleTest)
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
