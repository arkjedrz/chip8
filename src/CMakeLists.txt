cmake_minimum_required(VERSION 3.22)
project(Chip8)

# Global settings.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic)

# Library.
set(LIB_NAME ${CMAKE_PROJECT_NAME}Core)

set(LIB_SRC_FILES
    chip8.cpp
    game.cpp
    sdl.cpp
    timer.cpp
)

add_library(${LIB_NAME}
    ${LIB_SRC_FILES}
)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${LIB_NAME}
    CONAN_PKG::sdl
)

# Executable. Won't be accessible for testing.
set(EXE_NAME ${CMAKE_PROJECT_NAME})

set(EXE_SRC_FILES
    main.cpp
)

add_executable(${EXE_NAME}
    ${EXE_SRC_FILES}
)

target_link_libraries(${EXE_NAME}
    ${LIB_NAME}
    CONAN_PKG::cli11
)