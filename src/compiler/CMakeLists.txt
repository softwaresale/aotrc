
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(aotrc_compiler
        state_graph.cc state_graph.h
        match_function.cc match_function.h
        compiler_context.cc compiler_context.h)

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(aotrc_compiler ${llvm_libs})