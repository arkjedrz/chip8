cmake_minimum_required(VERSION 3.22)
project(Chip8Tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic)

find_package(GTest REQUIRED)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_opcodes.cpp
)

add_executable(Chip8Tests
    ${SRC_FILES}
)

target_link_libraries(Chip8Tests
    Chip8Core
    gtest::gtest
)
