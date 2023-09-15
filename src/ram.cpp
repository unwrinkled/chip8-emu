#include "ram.h"

Opcode::Opcode(u16 op) : m_opcode(op) {}

void Opcode::print() const noexcept {
    std::cout << std::hex << static_cast<unsigned int>(m_opcode) << std::dec << std::endl;
}
