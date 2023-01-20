cmake_minimum_required(VERSION 3.22)
project(Chip8)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(TESTING OFF)

add_subdirectory("src/")
if (TESTING)
    add_subdirectory("tests/")
endif()
