add_executable(chip8 chip8.cpp)

target_link_libraries(chip8 PRIVATE chip8_lib Microsoft.GSL::GSL)
