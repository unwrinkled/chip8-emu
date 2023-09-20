#ifndef C8_COMMANDS_H
#define C8_COMMANDS_H

#include "common.h"
#include "cpu.h"
#include "display.h"
#include "ram.h"
#include "registers.h"

#include <stack>

namespace commands {

void clear_or_return(const Opcode &op, Display &display, std::stack<u16> &stack, Registers &regs);

inline void jump(const Opcode &op, Registers &regs) noexcept {
    regs.set_pc(op.get_12bits());
}

inline void call(const Opcode &op, std::stack<u16> &stack, Registers &regs) {
    stack.push(regs.get_pc());
    regs.set_pc(op.get_12bits());
}

inline void if_reg_not_eq_value(const Opcode &op, Registers &regs) noexcept {
    if (regs.at(op.get_nibble(2)) == op.get_byte(0)) {
        regs.incr_pc();
    }
}

inline void if_reg_eq_value(const Opcode &op, Registers &regs) noexcept {
    if (regs.at(op.get_nibble(2)) != op.get_byte(0)) {
        regs.incr_pc();
    }
}

inline void if_reg_not_eq_reg(const Opcode &op, Registers &regs) noexcept {
    if (regs.at(op.get_nibble(2)) == regs.at(op.get_nibble(1))) {
        regs.incr_pc();
    }
}

inline void set_reg_value(const Opcode &op, Registers &regs) noexcept {
    auto reg = op.get_nibble(2);
    regs.at(reg) = op.get_byte(0);
}

inline void add_to_reg(const Opcode &op, Registers &regs) noexcept {
    auto reg = op.get_nibble(2);
    regs.at(reg) += op.get_byte(0);
}

void reg_operations(const Opcode &op, Registers &regs) noexcept;
inline void if_reg_eq_reg(const Opcode &op, Registers &regs) noexcept {
    if (regs.at(op.get_nibble(2)) != regs.at(op.get_nibble(1))) {
        regs.incr_pc();
    }
}

inline void set_index(const Opcode &op, Registers &regs) noexcept {
    regs.set_index(op.get_12bits());
}

void jump_add_plus_v0(const Opcode &op, Registers &regs) noexcept;

inline void random_number(const Opcode &op, Registers &regs, Rng &rng) noexcept {
    u8 rand_num = rng.gen() & op.get_byte(0);
    regs.at(op.get_nibble(2)) = rand_num;
}

void load_sprite(const Opcode &op, Registers &regs, RAM<> &memory, Display &display) noexcept;

void key_press_operations(const Opcode &op, Registers &regs, Display &display) noexcept;

void other_operations(const Opcode &op, Registers &regs, RAM<> &memory, Display &display, i8 &key_pressed) noexcept;

} // namespace commands

#endif
