cmake_minimum_required(VERSION 3.20)
project(path-tracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRCS "src/*.cpp")

add_executable(main)
target_sources(main PRIVATE ${SRCS})
target_include_directories(main PUBLIC include)

target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wsign-conversion
    -pedantic-errors
    -g
)

find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)
target_compile_definitions(main PRIVATE SDL_MAIN_USE_CALLBACKS)
