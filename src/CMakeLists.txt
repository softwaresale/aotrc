
# Add in finite automata library
add_subdirectory(fa)

# Add in the regex parser library
add_subdirectory(parser)

add_executable(aotrc_cpp main.cpp)
target_link_libraries(aotrc_cpp PUBLIC aotrc_fa aotrc_parser)