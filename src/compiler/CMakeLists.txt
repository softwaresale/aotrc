
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(aotrc_compiler
        compiler_context.cc compiler_context.h
        match_function_state.cc match_function_state.h
        match_function.cc match_function.h
        code_gen.cc code_gen.h
        lib_linker.cc lib_linker.h
        build_pattern_func.cc build_pattern_func.h)

# llvm_map_components_to_libnames(llvm_libs support core irreader codegen target passes x86)
execute_process(COMMAND llvm-config --libs all OUTPUT_VARIABLE llvm_libs)
string(STRIP ${llvm_libs} llvm_libs_stripped)
target_link_libraries(aotrc_compiler ${llvm_libs_stripped})