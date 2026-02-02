add_custom_command(
  OUTPUT shaders/fragment_shader.h
  COMMAND file-embedder ARGS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.frag fragment_shader
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders
  DEPENDS file-embedder ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.frag
)

add_custom_command(
  OUTPUT shaders/vertex_shader.h
  COMMAND file-embedder ARGS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.vert vertex_shader
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders
  DEPENDS file-embedder ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.vert
)

add_library(chip8_lib STATIC
  ram.cpp
  display.cpp
  cpu.cpp
  registers.cpp
  commands.cpp
  shaders/fragment_shader.h
  shaders/vertex_shader.h
)

target_compile_definitions(chip8_lib PUBLIC $<$<CONFIG:Debug>:CH_DEBUG>)
target_compile_features(chip8_lib PUBLIC cxx_std_20)
target_link_libraries(chip8_lib PUBLIC glad glfw Microsoft.GSL::GSL glm::glm)
target_include_directories(chip8_lib PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_executable(chip8 chip8.cpp)
target_link_libraries(chip8 PRIVATE chip8_lib)
