
# Configure some antlr stuff

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DANTLR4CPP_STATIC)

include(ExternalAntlr4Cpp.cmake)
include_directories(${ANTLR4_INCLUDE_DIRS})
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.9.3-complete.jar)
find_package(ANTLR REQUIRED)
