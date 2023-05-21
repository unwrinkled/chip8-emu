cmake_minimum_required(VERSION 3.26.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_PREFIX_PATH "C:/Users/death/dev/tools/GLFW")

project(chip8)

find_package(glfw3 CONFIG REQUIRED)

add_library(glad STATIC glad/gl.c)
target_include_directories(glad PUBLIC glad/include)

add_executable(chip8 src/main.cpp)
target_compile_features(chip8 PRIVATE cxx_std_20)
target_compile_options(chip8 PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,,-Wall -Wextra>)
target_link_libraries(chip8 PRIVATE glad glfw)
