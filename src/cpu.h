#ifndef C8_CPU_H
#define C8_CPU_H

#include "common.h"
#include "display.h"
#include "ram.h"
#include "registers.h"

#include <array>
#include <filesystem>
#include <random>
#include <stack>

class Rng {
  public:
    Rng();

    unsigned int gen();

  private:
    std::random_device m_rd;
    std::mt19937 m_engine;
    std::uniform_int_distribution<unsigned int> m_dist;
}; // for random number generation

class CPU {
  public:
    CPU(const std::filesystem::path &rom_file);

    void instr_cycle(double dt);

    [[nodiscard]] bool should_terminate() const {
        return m_display.should_close();
    }

  private:
    void fetch_decode_execute();

    RAM<> m_memory;
    std::stack<u16> m_stack;

    Registers m_regs;
    double m_time_passed;

    Display m_display;
    Rng m_rng;

    // key currently pressed or -1 if not pressed
    i8 m_key_pressed;
};

#endif
