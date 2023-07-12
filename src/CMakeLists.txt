find_package(glfw3 CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

set(H_FILE_LOC ${PROJECT_SOURCE_DIR}/include/chip8)
add_library(chip8_lib STATIC
  ${H_FILE_LOC}/common.h 
  ${H_FILE_LOC}/memory.h 
  memory.cpp
  ${H_FILE_LOC}/display.h
  display.cpp
)

target_compile_definitions(chip8_lib PUBLIC $<$<CONFIG:Debug>:CH_DEBUG>)
target_compile_options(chip8_lib PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,,-Wall -Wextra>)
target_compile_features(chip8_lib PUBLIC cxx_std_20)
target_include_directories(chip8_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(chip8_lib PRIVATE glad glfw Microsoft.GSL::GSL)
