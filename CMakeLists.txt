cmake_minimum_required(VERSION 3.26.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(chip8)

add_executable(chip8 src/main.cpp)
target_compile_features(chip8 PRIVATE cxx_std_20)
target_compile_options(chip8 PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,,-Wall -Wextra -Wpedantic>)
