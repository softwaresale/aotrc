
add_library(aotrc_fa
        transition_table.cc transition_table.h
        nfa.cc nfa.h
        nfa_builders.cc nfa_builders.h
        dfa.cc dfa.h
        graphviz_renderer.cc graphviz_renderer.h
        transition_edge.cc transition_edge.h)
target_include_directories(aotrc_fa INTERFACE ${CMAKE_SOURCE_DIR})
