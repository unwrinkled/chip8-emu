#ifndef C8_RAM_H
#define C8_RAM_H

#include "common.h"

#include <gsl/assert>
#include <gsl/gsl>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

class Opcode {
  public:
    explicit Opcode(u16 op);

    Opcode &operator=(u16 op) {
        m_opcode = op;
        return *this;
    }

    // pos is from right to left starting from 0
    [[nodiscard]] inline u8 get_nibble(u8 pos) const noexcept {
        Expects(pos >= 0 && pos <= 3);
        return m_opcode >> (pos * m_nibble) & m_nibble_mask;
    }

    [[nodiscard]] inline u8 get_byte(u8 pos) const noexcept {
        Expects(pos == 0 || pos == 1);
        return m_opcode >> (pos * m_byte) & m_byte_mask;
    }

    [[nodiscard]] inline u16 get_12bits() const noexcept {
        return (static_cast<u16>(get_nibble(2)) << m_byte) | get_byte(0);
    }
    void print() const noexcept;

  private:
    u16 m_opcode;

    static constexpr u8 m_nibble = 4;
    static constexpr u8 m_nibble_mask = 0x0f;
    static constexpr u8 m_byte = 8;
    static constexpr u8 m_byte_mask = 0xff;
};

constexpr u8 bytes_per_ch = 5;
constexpr u16 default_memory_size = 4096;
constexpr u8 font_size = 80;
constexpr u16 font_start = 0x50;
constexpr u16 rom_start = 0x200;

template <u16 N = default_memory_size>
class RAM {
  public:
    explicit RAM(const std::filesystem::path &filename) {
        auto rom_file = std::ifstream{ filename, std::ios::binary | std::ios::in };
        rom_file.exceptions(std::ifstream::failbit);
        rom_file.exceptions(std::ifstream::goodbit);

        // load rom file to memory
        auto rom_finish = std::copy(std::istreambuf_iterator(rom_file.rdbuf()), std::istreambuf_iterator<char>(), m_data.begin() + rom_start);
        rom_file.clear(std::ifstream::eofbit);
        rom_file.exceptions(std::ifstream::failbit);

        // determine rom file's size
        m_rom_size = std::distance(m_data.begin() + rom_start, rom_finish);

        // load default font
        constexpr std::array<u8, font_size> default_font{
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
        std::copy(default_font.begin(), default_font.end(), m_data.begin() + font_start);
    }

    [[nodiscard]] inline Opcode fetch(u16 pc) const noexcept {
        Expects(pc >= 0 && pc + 1 < N);
        // NOLINTNEXTLINE(*magic-numbers*): 8 bits
        return Opcode{ (static_cast<u16>(m_data.at(pc)) << 8) | static_cast<u16>(m_data.at(pc + 1)) };
    }

    [[nodiscard]] inline std::vector<u8> get_sprite(u16 i, u8 n) const noexcept {
        Expects(i >= 0 && i < N);
        Expects(i + n < N);

        std::vector<u8> vec;
        vec.insert(vec.begin(), std::next(m_data.begin(), i), std::next(m_data.begin(), i + n));

        return vec;
    }

    [[nodiscard]] inline u16 get_font_addr(u8 ch) const noexcept {
        Expects(ch <= 0x0f);

        return font_start + ch * bytes_per_ch;
    }

    inline void store_bcd(u16 i, u8 vx) noexcept {
        Expects(i + 2 < N);

        constexpr int base10 = 10;
        m_data.at(i) = vx / (base10 * base10);
        m_data.at(i + 1) = (vx / base10) % base10;
        m_data.at(i + 2) = vx % base10;
    }

    void store(u16 i, u8 vx, gsl::span<u8> &&regs) noexcept {
        Expects(i + vx <= N);
        std::copy(gsl::begin(regs), std::next(gsl::begin(regs), vx + 1), std::next(m_data.begin(), i));
    }

    void load(u16 i, u8 vx, gsl::span<u8> &&regs) noexcept {
        Expects(i + vx <= N);
        std::copy(std::next(m_data.begin(), i), std::next(m_data.begin(), i + vx + 1), gsl::begin(regs));
    }

#ifdef CH_DEBUG
    void print_rom() const {
        auto prev_fill = std::cout.fill('0');
        std::cout << std::hex;
        for (auto i = 0; i < m_rom_size / 2; ++i) {
            std::cout << std::setw(2) << static_cast<int>(m_data.at(rom_start + i * 2)) << std::setw(2) << static_cast<int>(m_data.at(rom_start + i * 2 + 1)) << std::endl;
        }
        std::cout.fill(prev_fill);
        std::cout << std::dec;
    }

    void print_font() const {
        auto end = m_data.begin() + font_start + font_size;
        auto count = 0;
        auto prev_fill = std::cout.fill('0');
        constexpr auto bytes_per_ch = 5;
        std::cout << std::hex;
        for (auto it = m_data.begin() + font_start; it < end; ++it) {
            std::cout << std::setw(2) << static_cast<int>(*it) << ' ';
            if (++count % bytes_per_ch == 0) {
                std::cout << std::endl;
            }
        }
        std::cout.fill(prev_fill);
        std::cout << std::dec;
    }

    [[nodiscard]] u8 fetch_i(u16 i) const noexcept {
        Expects(i < N);
        return m_data.at(i);
    }
#endif

  private:
    std::array<u8, N> m_data;
    u16 m_rom_size;
};

#endif
