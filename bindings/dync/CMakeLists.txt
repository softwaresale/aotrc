
add_library(dynamic_c SHARED
        dynamic_binding.c dynamic_binding.h)

set_property(TARGET dynamic_c PROPERTY C_STANDARD 11)
target_link_libraries(dynamic_c PRIVATE dl)
