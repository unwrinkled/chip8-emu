#include <chip8/chip8.h>

#include <gsl/gsl>

#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "USAGE: chip8 romfile" << std::endl;
        return -1;
    }

    [[maybe_unused]] auto args = gsl::make_span(argv, argc);

    try {
        run_chip8(gsl::at(args, 1));
        // run_chip8("roms/IBM_Logo.ch8");
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}