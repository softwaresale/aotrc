cmake_minimum_required(VERSION 3.18.2)
project(aotrc)

set(CMAKE_CXX_STANDARD 17)

# Setup any 3rd party dependencies
add_subdirectory(thirdparty)

# Build all sources
add_subdirectory(src)

# Build tests
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)
add_subdirectory(tests)

# Add directory for specific language bindings
add_subdirectory(bindings)
