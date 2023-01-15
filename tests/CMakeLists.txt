cmake_minimum_required(VERSION 3.22)
project(cpp_templateTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_hello.cpp
)

add_executable(cpp_templateTests
    ${SRC_FILES}
)

target_link_libraries(cpp_templateTests
    libcpp_template
    CONAN_PKG::gtest
)
