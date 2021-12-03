
# Add in finite automata library
add_subdirectory(fa)

add_executable(aotrc_cpp main.cpp)
target_link_libraries(aotrc_cpp PUBLIC aotrc_fa)