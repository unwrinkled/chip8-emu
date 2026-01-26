add_library(chip8_lib STATIC
  ram.cpp
  display.cpp
  cpu.cpp
  registers.cpp
  commands.cpp
)

target_compile_definitions(chip8_lib PUBLIC $<$<CONFIG:Debug>:CH_DEBUG>)
target_compile_features(chip8_lib PUBLIC cxx_std_20)
target_link_libraries(chip8_lib PUBLIC glad glfw Microsoft.GSL::GSL glm::glm)

add_executable(chip8 chip8.cpp)
target_link_libraries(chip8 PRIVATE chip8_lib)
