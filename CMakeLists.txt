cmake_minimum_required(VERSION 3.20)

project(dll_injector LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "This project only builds on Windows.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(inject
    src/cli.cpp
    src/error.cpp
    src/injector.cpp
    src/main.cpp
    src/process.cpp
)

target_include_directories(inject PRIVATE include)

if(MSVC)
    target_compile_options(inject PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(inject PRIVATE -Wall -Wextra -Wpedantic)
endif()
