
add_library(aotrc_input
        aotrc_input_parser.cc aotrc_input_parser.h)

target_link_libraries(aotrc_input PRIVATE yaml-cpp::yaml-cpp)