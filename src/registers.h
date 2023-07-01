#ifndef C8_REGISTERS_H
#define C8_REGISTERS_H

#include <array>

#include "common.h"

constexpr int num_of_registers = 16;

class Registers {
  public:
    Registers();

  private:
    u16 m_pc;
    u16 m_index;
    std::array<u8, num_of_registers> m_regs;
};

#endif
