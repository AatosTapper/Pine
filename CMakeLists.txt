cmake_minimum_required(VERSION 3.18)

project(Pine)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE files
    "core/include/*.h"
    "core/src/*.cpp"
)

set(tinyxml2_sources "dep/tinyxml2/tinyxml2.cpp")

add_executable(main ${files} ${tinyxml2_sources})

target_include_directories(main PUBLIC core/include dep)

target_compile_options(main PRIVATE 
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion -Wsign-conversion
)
