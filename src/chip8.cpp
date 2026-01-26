#include "cpu.h"

#include <iostream>
#include <filesystem>

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

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "USAGE: chip8 romfile" << std::endl;
        return -1;
    }

    std::filesystem::path rom_file = argv[1];

    try {
        CPU cpu(rom_file);

        auto current_time = glfwGetTime();
        while (!cpu.should_terminate()) {
            auto new_time = glfwGetTime();
            auto frame_time = new_time - current_time;
            current_time = new_time;

            cpu.instr_cycle(frame_time);
        }
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
