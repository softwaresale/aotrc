
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(aotrc_compiler
        compiler.cc compiler.h
        program.cc program.h
        instruction.cc instruction.h
        program_state.cc program_state.h)

execute_process(COMMAND llvm-config --libs all OUTPUT_VARIABLE llvm_libs)
execute_process(COMMAND llvm-config --includedir OUTPUT_VARIABLE llvm_include_dirs)
string(STRIP ${llvm_libs} llvm_libs_stripped)
string(STRIP ${llvm_include_dirs} llvm_include_stripped)
# TODO this following line is breaking on my system
target_include_directories(aotrc_compiler PUBLIC ${llvm_include_stripped})
target_link_libraries(aotrc_compiler ${llvm_libs_stripped} aotrc_parser)