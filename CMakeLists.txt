cmake_minimum_required(VERSION 3.18)

project(Pine)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE src_files
    "core/src/*.cpp"
)

add_executable(main ${src_files} dep/tinyxml2/tinyxml2.cpp)

add_library(glad STATIC dep/glad/glad.c)
target_include_directories(glad PUBLIC dep)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    find_package(glfw3 REQUIRED)
    set(GLFW_LIB glfw)

    find_package(Lua REQUIRED)
    include_directories(${LUA_INCLUDE_DIR})

    set(CMAKE_CXX_COMPILER clang++)

    target_compile_options(main PRIVATE 
        -O3
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion -Wsign-conversion
    )
else () # windows
    set(GLFW_LIBRARY_DIR "C:/Program Files/glfw-3.4/glfw-3.4.bin.WIN64")

    # TODO: fucking windows bullshit

    #find_package(glfw3 REQUIRED)
    set(GLFW_LIB glfw3dll)

    set(CMAKE_CXX_COMPILER "cl")

    target_compile_options(main PRIVATE 
        "/W3" "/O3" "/DNDEBUG" "/Zi" "/Gy"
    )
endif()

target_precompile_headers(main PRIVATE core/include/pch.h)

target_include_directories(main PUBLIC core/include dep)
target_link_directories(main PUBLIC ${GLFW_LIBRARY_DIR})

find_package(OpenGL REQUIRED)

target_link_libraries(main ${GLFW_LIB} ${LUA_LIBRARIES} OpenGL::GL glad)