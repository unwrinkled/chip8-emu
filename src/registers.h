#ifndef C8_REGISTERS_H
#define C8_REGISTERS_H

#include "common.h"

#include <gsl/span>

#include <array>

class Registers {
  public:
    static inline constexpr u8 m_number_of_registers = 16;

    Registers(u16 rom_start);

    // update timer registers (timer and sound)
    void update_timers(double dt);

    // next instruction
    inline void incr_pc() {
        m_pc += 2;
    }

    // previous instruction
    inline void decr_pc() {
        m_pc -= 2;
    }

    // increase index by offset
    inline void add_index(const u16 offset) {
        m_index += offset;
    }

    // jump to an addr
    inline void set_pc(const u16 addr) {
        m_pc = addr;
    }

    inline void set_index(const u16 addr) {
        m_index = addr;
    }

    inline void set_timer(const double time) {
        m_timer = time;
    }

    inline void set_sound(const double sound) {
        m_sound = sound;
    }

    [[nodiscard]] inline u16 get_pc() const {
        return m_pc;
    }

    [[nodiscard]] u16 get_index() const {
        return m_index;
    }

    [[nodiscard]] inline double get_timer() const {
        return m_timer;
    }

    gsl::span<u8> get_regs_span();

    inline u8 &at(size_t i) {
        return m_regs.at(i);
    }

  private:
    std::array<u8, m_number_of_registers> m_regs;
    u16 m_pc;    // program counter
    u16 m_index; // pointer to ram memory for sprites

    double m_timer;
    double m_sound;
};

#endif
