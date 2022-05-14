
add_library(aotrc_jit
        aotrc_jit_compiler.cc aotrc_jit_compiler.h)
target_link_libraries(aotrc_jit PUBLIC aotrc_compiler aotrc_parser)