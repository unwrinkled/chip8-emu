#include <chip8/chip8.h>

#include "cpu.h"

void run_chip8(const std::filesystem::path &rom_file) {
    CPU cpu(rom_file);

    auto current_time = glfwGetTime();
    while (!cpu.should_terminate()) {
        auto new_time = glfwGetTime();
        auto frame_time = new_time - current_time;
        current_time = new_time;

        cpu.instr_cycle(frame_time);
    }
}
