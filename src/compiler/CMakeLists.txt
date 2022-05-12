
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(aotrc_compiler
        instruction.cc instruction.h
        dfa_compiler.cc dfa_compiler.h
        program.cc program.h
        instruction_translator.cc instruction_translator.h
        util.cc util.h
        program_translator.h program_translator.cc
        regex_compiler.h regex_compiler.cc
        program_info_provider.cc program_info_provider.h
        aotrc_compiler.cc aotrc_compiler.h
        linker.cc linker.h
        full_match/full_match_dfa_compiler.cc full_match/full_match_dfa_compiler.h
        full_match/full_match_translator.cc full_match/full_match_translator.h
        full_match/full_match_program_info_provider.cc
        full_match/full_match_program_info_provider.h
        sub_match/sub_match_program_info_provider.cc sub_match/sub_match_program_info_provider.h
        sub_match/sub_match_dfa_compiler.cc sub_match/sub_match_dfa_compiler.h
        searching/search_program_info_provider.cc searching/search_program_info_provider.h
        searching/search_dfa_compiler.cc searching/search_dfa_compiler.h
        searching/search_translator.cc searching/search_translator.h)

execute_process(COMMAND llvm-config --libs all OUTPUT_VARIABLE llvm_libs)
execute_process(COMMAND llvm-config --includedir OUTPUT_VARIABLE llvm_include_dirs)
string(STRIP ${llvm_libs} llvm_libs_stripped)
string(STRIP ${llvm_include_dirs} llvm_include_stripped)
# TODO this following line is breaking on my system
target_include_directories(aotrc_compiler PUBLIC ${llvm_include_stripped})
target_link_libraries(aotrc_compiler ${llvm_libs_stripped} aotrc_parser)
