cmake_minimum_required(VERSION 3.18)

project(Pine)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

    set(EXTRA_OPT 
        -march=native
        -mtune=native
        -funroll-loops
        -ffast-math
        -fomit-frame-pointer
    )
    target_compile_options(main PRIVATE 
        -Ofast
        -flto
        -Wall
        -Wextra
        -Wpedantic
        ${EXTRA_OPT}
    )
    target_precompile_headers(main PRIVATE core/include/pch.h)

else () # windows
    set(GLFW_LIBRARY_DIR "C:/Program Files/glfw-3.4/lib-vc2022")  # Adjust the path as necessary

    find_package(Lua REQUIRED)
    include_directories("C:/Program Files/lua/lib/include")

    set(CMAKE_CXX_COMPILER "cl")

    target_compile_options(main PRIVATE 
        "/W3"
        "/O2"
        "/GL"
        "/DNDEBUG"
        "/Zi"
        "/arch:AVX2"
        "/fp:fast"
    )

    target_link_options(main PRIVATE
        "/LTCG"
        "/OPT:REF"
        "/OPT:ICF"
    )
endif()

set_target_properties(main PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(main PROPERTIES LINK_SEARCH_END_STATIC 1)

target_include_directories(main PUBLIC core/include core/include/rendering dep)
target_link_directories(main PUBLIC ${GLFW_LIBRARY_DIR})

find_package(OpenGL REQUIRED)

target_link_libraries(main ${GLFW_LIB} ${LUA_LIBRARIES} OpenGL::GL glad)
