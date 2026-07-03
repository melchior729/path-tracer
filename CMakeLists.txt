cmake_minimum_required(VERSION 3.20)
project(path-tracer LANGUAGES CXX CUDA)

set(CMAKE_CUDA_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES native)

file(GLOB SRCS "src/*.cpp" "src/*.cu")

add_executable(main)
target_sources(main PRIVATE ${SRCS})
target_include_directories(main PUBLIC include)

set(CXX_WARNINGS -Wall -Wextra -Werror -Wsign-conversion -pedantic-errors -g)
set(CUDA_WARNINGS
  -Xcompiler=-Wall,-Wextra,-Wsign-conversion
  -Xcudafe=--display_error_number
  --Werror=all-warnings
  -lineinfo
  --expt-relaxed-constexpr
)

target_compile_options(main PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_WARNINGS}>
    $<$<COMPILE_LANGUAGE:CUDA>:${CUDA_WARNINGS}>
)

set_target_properties(main PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

find_package(SDL3 REQUIRED)
target_link_libraries(main PRIVATE SDL3::SDL3)
target_compile_definitions(main PRIVATE SDL_MAIN_USE_CALLBACKS)
