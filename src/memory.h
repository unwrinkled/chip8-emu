#ifndef C8_MEMORY_H
#define C8_MEMORY_H

#include "common.h"

// #include <cassert>
// #include <cstring>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>

class Opcode {
  public:
    Opcode(u8 b0, u8 b1);
    void print() const;

  private:
    std::array<u8, 2> m_opcode;
};

class FontCharacter {
  public:
    FontCharacter(const u8 *start);
    void print() const;

  private:
    static constexpr int bytes_per_char = 5;

    std::array<u8, bytes_per_char> m_character;
};

constexpr int default_memory_size = 4096;

template <u16 N = default_memory_size>
class Memory {
  public:
    Memory() : m_data{ 0 }, m_font_start(0x50), m_rom_index{ 512 }, m_rom_size(0) {}

    bool load_rom(const char *filename) {
        auto rom_file = std::ifstream{ filename, std::ios::binary | std::ios::in };
        rom_file.read(reinterpret_cast<char *>(m_data) + m_rom_index, N - m_rom_index);
        m_rom_size = rom_file.gcount();

        return rom_file.eof();
    }

    void load_font_default() {
        u8 *start = m_data + m_font_start;
        std::memcpy(start, default_font, font_size);
    }

    Opcode fetch(u16 pc) const {
        return Opcode{ m_data[pc], m_data[pc + 1] };
    }

    FontCharacter get_font_char(u8 ch) const {
        assert((ch <= 0x0f) && "font character is too big");
        // assert((buffer != nullptr) && "font character buffer is null");

        return FontCharacter(m_data + m_font_start + ch * 5);
        // std::memcpy(buffer, m_data + m_font_start + ch * 5, 5);
    }

#ifdef CH_DEBUG
    void print_rom() const {
        auto prev_fill = std::cout.fill('0');
        std::cout << std::hex;
        for (auto i = 0; i < m_rom_size / 2; ++i) {
            std::cout << std::setw(2) << static_cast<int>(m_data[m_rom_index + i * 2]) << std::setw(2) << static_cast<int>(m_data[m_rom_index + i * 2 + 1]) << std::endl;
        }
        std::cout.fill(prev_fill);
        std::cout << std::dec;
    }

    void print_font() const {
        auto start = m_data + m_font_start;
        auto count = 0;
        auto prev_fill = std::cout.fill('0');
        std::cout << std::hex;
        for (auto it = start; it < start + font_size; ++it) {
            std::cout << std::setw(2) << static_cast<int>(*it) << ' ';
            if (++count % 5 == 0) {
                std::cout << std::endl;
            }
        }
        std::cout.fill(prev_fill);
        std::cout << std::dec;
    }
#endif

  private:
    // u8 m_data[N];
    std::array<u8, N> m_data;
    u16 m_font_start;
    u16 m_rom_index;
    u16 m_rom_size;

    static const u8 font_size = 80;
    static constexpr u8 default_font[font_size] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
};

#endif
