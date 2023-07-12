find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(chip8 chip8.cpp)

target_compile_options(chip8 PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,,-Wall -Wextra>)
target_link_libraries(chip8 PRIVATE glad chip8_lib Microsoft.GSL::GSL glm::glm)
