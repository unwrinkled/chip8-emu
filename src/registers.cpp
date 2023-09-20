#include "registers.h"

#include <gsl/span>

namespace {
constexpr double decrement_speed = 60.0;
}

Registers::Registers(u16 rom_start) : m_regs{ 0 }, m_pc(rom_start), m_index(0), m_timer(0.0), m_sound(0.0) {}

void Registers::update_timers(double dt) {
    double decrement_timers = dt * decrement_speed;
    if (m_timer > 0.0) {
        m_timer -= decrement_timers;
        if (m_timer < 0.0) {
            m_timer = 0.0;
        }
    }
    if (m_sound > 0.0) {
        m_sound -= decrement_timers;
        if (m_sound < 0.0) {
            m_sound = 0.0;
        }
    }
}

gsl::span<u8> Registers::get_regs_span() {
    return gsl::make_span(m_regs);
}
