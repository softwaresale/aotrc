cmake_minimum_required(VERSION 3.20.2)
project(atorc)

set(CMAKE_CXX_STANDARD 17)

# Setup any 3rd party dependencies
add_subdirectory(thirdparty)

add_subdirectory(src)
