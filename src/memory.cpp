#include "memory.h"
#include <cstring>

Opcode::Opcode(u8 b0, u8 b1) : m_opcode{ b0, b1 } {}

void Opcode::print() const {
    std::cout << std::hex << static_cast<int>(m_opcode[0]) << static_cast<int>(m_opcode[1]) << std::dec << std::endl;
}

FontCharacter::FontCharacter(const u8 *start) : m_character{ 0 } {
    std::memcpy(m_character.data(), start, bytes_per_char);
}

void FontCharacter::print() const {
    std::cout << std::hex;
    for (auto x : m_character) {
        std::cout << static_cast<int>(x) << ' ';
    }
    std::cout << std::dec << std::endl;
}