#include <chip8/memory.h>

constexpr u8 bits_per_byte = 8;
constexpr u8 byte_mask = 0xff;

Opcode::Opcode(u8 b0, u8 b1) : m_opcode{ b0, b1 } {}

bool Opcode::operator==(const u16 &rhs) const {
    return m_opcode.at(0) == (rhs >> bits_per_byte & byte_mask) && m_opcode.at(1) == (rhs & byte_mask);
}

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