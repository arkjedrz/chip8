cmake_minimum_required(VERSION 3.22)
project(cpp_template)

# Global settings.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic)

# Library.
set(LIB_NAME lib${CMAKE_PROJECT_NAME})

set(LIB_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp
)

add_library(${LIB_NAME}
    ${LIB_SRC_FILES}
)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${LIB_NAME}
    # Core functionality CONAN_PKGs here, eg:
    # CONAN_PKG::boost
)

# Executable. Won't be accessible for testing.
set(EXE_NAME ${CMAKE_PROJECT_NAME})

set(EXE_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${EXE_NAME}
    ${EXE_SRC_FILES}
)

target_link_libraries(${EXE_NAME}
    ${LIB_NAME}
    # Executable-related CONAN_PKGs here, eg:
    # CONAN_PKG::cli11
)