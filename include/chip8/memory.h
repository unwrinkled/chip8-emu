#ifndef C8_MEMORY_H
#define C8_MEMORY_H

#include "common.h"

#include <gsl/gsl>

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

class Opcode {
  public:
    Opcode(u8 b0, u8 b1);
    void print() const;

  private:
    std::array<u8, 2> m_opcode;
};

constexpr int bytes_per_ch = 5;

class FontCharacter {
  public:
    explicit FontCharacter(const std::array<u8, bytes_per_ch> &arr);
    void print() const;

  private:
    std::array<u8, bytes_per_ch> m_character;
};

constexpr int default_memory_size = 4096;

template <u16 N = default_memory_size>
class Memory {
  public:
    explicit Memory(const std::string &filename) {
        auto rom_file = std::ifstream{ filename, std::ios::binary | std::ios::in };
        rom_file.exceptions(std::ifstream::failbit);
        rom_file.exceptions(std::ifstream::goodbit);

        // load rom file to memory
        auto rom_finish = std::copy(std::istream_iterator<u8>(rom_file), std::istream_iterator<u8>(), m_data.begin() + m_rom_start);
        rom_file.clear(std::ifstream::eofbit);
        rom_file.exceptions(std::ifstream::failbit);

        // determine rom file's size
        m_rom_size = std::distance(m_data.begin() + m_rom_start, rom_finish);

        // load default font
        constexpr std::array<u8, m_font_size> default_font{
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
        std::copy(default_font.begin(), default_font.end(), m_data.begin() + m_font_start);
    }

    [[nodiscard]] inline Opcode fetch(u16 pc) const {
        Expects(pc + 1 < N);
        return Opcode{ m_data.at(pc), m_data.at(pc + 1) };
    }

    [[nodiscard]] inline FontCharacter get_font_char(u8 ch) const {
        Expects(ch <= 0x0f);

        std::array<u8, bytes_per_ch> ch_arr{ 0 };
        auto start = m_data.begin() + m_font_start + ch * bytes_per_ch;
        std::copy(start, start + bytes_per_ch, ch_arr.begin());

        return FontCharacter(ch_arr);
    }

#ifdef CH_DEBUG
    void print_rom() const {
        auto prev_fill = std::cout.fill('0');
        std::cout << std::hex;
        for (auto i = 0; i < m_rom_size / 2; ++i) {
            std::cout << std::setw(2) << static_cast<int>(m_data.at(m_rom_start + i * 2)) << std::setw(2) << static_cast<int>(m_data.at(m_rom_start + i * 2 + 1)) << std::endl;
        }
        std::cout.fill(prev_fill);
        std::cout << std::dec;
    }

    void print_font() const {
        auto end = m_data.begin() + m_font_start + m_font_size;
        auto count = 0;
        auto prev_fill = std::cout.fill('0');
        constexpr auto bytes_per_ch = 5;
        std::cout << std::hex;
        for (auto it = m_data.begin() + m_font_start; it < end; ++it) {
            std::cout << std::setw(2) << static_cast<int>(*it) << ' ';
            if (++count % bytes_per_ch == 0) {
                std::cout << std::endl;
            }
        }
        std::cout.fill(prev_fill);
        std::cout << std::dec;
    }
#endif

  private:
    std::array<u8, N> m_data;
    u16 m_rom_size;

    static constexpr u8 m_font_size = 80;
    static constexpr u16 m_font_start = 0x50;
    static constexpr u16 m_rom_start = 0x200;
};

#endif
