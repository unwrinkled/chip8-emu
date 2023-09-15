#include "ram.h"

Opcode::Opcode(u16 op) : m_opcode(op) {}

void Opcode::print() const noexcept {
    std::cout << std::hex << static_cast<unsigned int>(m_opcode) << std::dec << std::endl;
}

// FontCharacter::FontCharacter(const std::array<u8, bytes_per_ch> &arr) : m_character(arr) {}
//
// void FontCharacter::print() const {
//     std::cout << std::hex;
//     for (auto x : m_character) {
//         std::cout << static_cast<int>(x) << ' ';
//     }
//     std::cout << std::dec << std::endl;
// }
