cmake_minimum_required(VERSION 3.22)
project(Chip8)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

option(TESTING OFF)
option(CLANG_TIDY OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy "-checks=-*,modernize-*,clang-analyzer-*,clang-analyzer-cplusplus*,cppcoreguidelines-*,-modernize-use-trailing-return-type,-cppcoreguidelines-avoid-magic-numbers")
endif()

add_subdirectory("src/")
if (TESTING)
    add_subdirectory("tests/")
endif()
