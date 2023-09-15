cmake_minimum_required(VERSION 3.24)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

# set(GLAD_PROFILE core)

project(chip8
  VERSION 0.1.0
  DESCRIPTION "My attempt at writing a CHIP8 emulator"
)

add_subdirectory(extern)
add_subdirectory(src lib)
add_subdirectory(chip8 bin)

set_target_properties(chip8_lib chip8
  PROPERTIES EXPORT_COMPILE_COMMANDS YES
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

if (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  option(PACKAGE_TESTS "Build tests" ON)
endif()

if (PACKAGE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()
