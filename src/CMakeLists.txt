
# Add antlr dependencies because it's used in parser and input
find_program(antlr4 4.9.3)
find_package(antlr4-runtime REQUIRED 4.9.3)
find_package(antlr4-generator REQUIRED)

set(ANTLR4_JAR_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/antlr/antlr-4.9.3-complete.jar)

# Add compile options to pick up more errors
add_compile_options(-Wall -Werror)

# Add in finite automata library
add_subdirectory(fa)

# Add in the regex parser library
add_subdirectory(parser)

# Add in the compiler
add_subdirectory(compiler)

# Add in the directory that defines the input file definition
add_subdirectory(input)
add_executable(aotrc
        main.cpp
        args_parser.cc args_parser.h)
target_link_libraries(aotrc PUBLIC aotrc_fa aotrc_parser aotrc_compiler aotrc_input)