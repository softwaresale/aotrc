cmake_minimum_required(VERSION 3.20.2)
project(atorc)

set(CMAKE_CXX_STANDARD 17)

# Setup any 3rd party dependencies
add_subdirectory(thirdparty)

# Build all sources
add_subdirectory(src)

# Build tests
add_subdirectory(tests)

# Add directory for specific language bindings
add_subdirectory(bindings)
