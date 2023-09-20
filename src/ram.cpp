#include "ram.h"

Opcode::Opcode(u16 op) : m_opcode(op) {}

Opcode &Opcode::operator=(u16 op) {
    m_opcode = op;
    return *this;
}

void Opcode::print() const noexcept {
    std::cout << std::hex << static_cast<unsigned int>(m_opcode) << std::dec << std::endl;
}
