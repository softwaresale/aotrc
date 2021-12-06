
# Add in finite automata library
add_subdirectory(fa)

# Add in the regex parser library
add_subdirectory(parser)

# Add in the compiler
add_subdirectory(compiler)

add_executable(aotrc
        main.cpp
        args_parser.cc args_parser.h)
target_link_libraries(aotrc PUBLIC aotrc_fa aotrc_parser aotrc_compiler)