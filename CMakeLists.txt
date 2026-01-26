cmake_minimum_required(VERSION 3.24)

if ("${CMAKE_SOURCE_DIR}" EQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "Building in the source directory is not allowed.")
endif()

option(BUILD_TESTS "Build tests" OFF)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(chip8
  VERSION 0.1.0
  DESCRIPTION "My attempt at writing a CHIP8 emulator"
)

add_subdirectory(extern)
add_subdirectory(src lib)
add_subdirectory(chip8 bin)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
