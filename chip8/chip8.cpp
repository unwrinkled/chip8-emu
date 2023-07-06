#include <chip8/common.h>
#include <chip8/memory.h>

#include <gsl/gsl>

#include <fstream>
#include <iomanip>
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "USAGE: chip8 filename" << std::endl;
        return -1;
    }

    auto args = gsl::make_span(argv, argc);

    try {
        Memory memory(gsl::at(args, 1));
        const u16 pc = 512;
        Opcode opcode = memory.fetch(pc);
        opcode.print();

        const u8 ch = 0x0a;
        FontCharacter character = memory.get_font_char(ch);
        character.print();
    } catch (const std::ios_base::failure &e) {
        std::cerr << e.what() << std::endl;
        // std::cerr << "Can't read whole ROM file to memory. Exiting..." << std::endl;
        return -1;
    }

    return 0;
}