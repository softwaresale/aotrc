
# Add compile options to pick up more errors
add_compile_options(-Wall -Werror)

# Add in the directory that defines the input file definition
add_subdirectory(input)

# Add in the regex parser library
add_subdirectory(parser)

# Add in finite automata library
add_subdirectory(fa)

# Add in the compiler
add_subdirectory(compiler)

# JIT compilation support
add_subdirectory(jit)

add_executable(aotrc
        main.cc
        args_parser.cc args_parser.h)
target_link_libraries(aotrc PUBLIC aotrc_fa aotrc_parser aotrc_compiler aotrc_input)

# Useful for testing
set(AOTRC_ALL_LIBS aotrc_fa aotrc_parser aotrc_compiler aotrc_input)
