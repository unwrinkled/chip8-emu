#include <chip8/memory.h>

Opcode::Opcode(u8 b0, u8 b1) : m_opcode{ b0, b1 } {}

void Opcode::print() const {
    std::cout << std::hex << static_cast<int>(m_opcode.at(0)) << static_cast<int>(m_opcode.at(1)) << std::dec << std::endl;
}

FontCharacter::FontCharacter(const std::array<u8, bytes_per_ch> &arr) : m_character(arr) {}

void FontCharacter::print() const {
    std::cout << std::hex;
    for (auto x : m_character) {
        std::cout << static_cast<int>(x) << ' ';
    }
    std::cout << std::dec << std::endl;
}