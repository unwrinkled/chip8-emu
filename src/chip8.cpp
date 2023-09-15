#include <chip8/chip8.h>

#include "display.h"
#include "ram.h"

#include <algorithm>
#include <gsl/span_ext>
#include <iostream>
#include <limits>
#include <random>
#include <stack>

void run_chip8(const std::filesystem::path &rom_file) {
    Display display;
    RAM memory(rom_file);

    std::random_device r;
    std::mt19937 e1(r());
    std::uniform_int_distribution<unsigned int> uni_dist(0x00, std::numeric_limits<u8>::max());

    constexpr bool old_shift = false;
    constexpr bool old_jump_offset = true;

    constexpr u16 instr_per_sec = 500;
    constexpr double sec_per_instr = 1.0 / instr_per_sec;

    // NOLINTBEGIN(*magic-numbers*)
    std::stack<u16> stack;
    std::array<u8, 16> reg_v{ 0 };
    u16 i = 0;
    double timer = 0.0;
    double sound = 0.0;
    u16 pc = rom_start;

    i8 key_pressed = -1;

    double time_passed = 0.0;
    auto current_time = glfwGetTime();
    while (!display.should_close()) {
        auto new_time = glfwGetTime();
        auto frame_time = new_time - current_time;
        current_time = new_time;
        time_passed += frame_time;

        double decrement_timers = frame_time * 60.0;
        if (timer > 0.0) {
            timer -= decrement_timers;
            if (timer < 0.0) {
                timer = 0.0;
            }
        }
        if (sound > 0.0) {
            sound -= decrement_timers;
            if (sound < 0.0) {
                sound = 0.0;
            }
        }

        if (time_passed >= sec_per_instr) {
            time_passed -= sec_per_instr;
            // fetch
            auto op = memory.fetch(pc);
            pc += 2;
            // op.print();
            // std::cout << "\r" << static_cast<unsigned int>(timer);

            // decode and execute
            switch (op.get_nibble(3)) {
                case (0): {
                    switch (op.get_12bits()) {
                        case (0x00e0): {
                            display.clear_screen();
                            break;
                        }
                        case (0x00ee): {
                            if (stack.empty()) {
                                throw std::runtime_error("trying to pop address from stack while empty");
                            }
                            pc = stack.top();
                            stack.pop();
                            break;
                        }
                        default: {
                            std::cout << "skip instruction\n";
                            return;
                        }
                    }
                    break;
                }

                case (0x1): {
                    // std::cout << "jumping\n";
                    pc = op.get_12bits();
                    break;
                }

                case (0x2): {
                    // std::cout << "calling subroutine\n";
                    stack.push(pc);
                    pc = op.get_12bits();
                    break;
                }

                case (0x3): {
                    // std::cout << "skip if vx == NN\n";
                    if (reg_v.at(op.get_nibble(2)) == op.get_byte(0)) {
                        pc += 2;
                    }
                    break;
                }

                case (0x4): {
                    // std::cout << "skip if vx != NN\n";
                    if (reg_v.at(op.get_nibble(2)) != op.get_byte(0)) {
                        pc += 2;
                    }
                    break;
                }

                case (0x5): {
                    // std::cout << "skip if vx == vy\n";
                    if (reg_v.at(op.get_nibble(2)) == op.get_nibble(1)) {
                        pc += 2;
                    }
                    break;
                }

                case (0x6): {
                    // std::cout << "set register\n";
                    auto reg = op.get_nibble(2);
                    reg_v.at(reg) = op.get_byte(0);
                    break;
                }

                case (0x7): {
                    // std::cout << "add NN to vx\n";
                    auto reg = op.get_nibble(2);
                    reg_v.at(reg) += op.get_byte(0);
                    break;
                }

                case (0x8): {
                    switch (op.get_nibble(0)) {
                        case (0x0): {
                            // std::cout << "set vx = vy\n";
                            reg_v.at(op.get_nibble(2)) = reg_v.at(op.get_nibble(1));
                            break;
                        }

                        case (0x1): {
                            // std::cout << "logical or\n";
                            reg_v.at(op.get_nibble(2)) |= reg_v.at(op.get_nibble(1));
                            reg_v.at(0xf) = 0;
                            break;
                        }

                        case (0x2): {
                            // std::cout << "logical and\n";
                            reg_v.at(op.get_nibble(2)) &= reg_v.at(op.get_nibble(1));
                            reg_v.at(0xf) = 0;
                            break;
                        }

                        case (0x3): {
                            // std::cout << "logical xor\n";
                            reg_v.at(op.get_nibble(2)) ^= reg_v.at(op.get_nibble(1));
                            reg_v.at(0xf) = 0;
                            break;
                        }

                        case (0x4): {
                            // std::cout << "add vy to vx and update carry flag\n";
                            auto &vx = reg_v.at(op.get_nibble(2));
                            auto vy = reg_v.at(op.get_nibble(1));

                            reg_v.at(0xf) = std::numeric_limits<u8>::max() - vx > vy ? 1 : 0;
                            vx += vy;
                            break;
                        }

                        case (0x5): {
                            // std::cout << "subtract vy from vx and update borrow flag\n";
                            auto &vx = reg_v.at(op.get_nibble(2));
                            auto vy = reg_v.at(op.get_nibble(1));

                            reg_v.at(0xf) = vx >= vy ? 1 : 0;
                            vx -= vy;
                            break;
                        }

                        case (0x6): {
                            // std::cout << "vx >> 1 and set vf to old shifted bit\n";
                            auto &vx = reg_v.at(op.get_nibble(2));
                            if (old_shift) {
                                vx = reg_v.at(op.get_nibble(1)); // optional
                            }
                            reg_v.at(0xf) = vx & 0x1;
                            vx >>= 1;
                            break;
                        }

                        case (0x7): {
                            // std::cout << "set vx = vy - vx and update borrow flag\n";
                            auto &vx = reg_v.at(op.get_nibble(2));
                            auto vy = reg_v.at(op.get_nibble(1));
                            reg_v.at(0xf) = vy >= vx ? 1 : 0;
                            vx = vy - vx;
                            break;
                        }

                        case (0xe): {
                            // std::cout << "vx << 1 and set vf to old shifted bit\n";
                            auto &vx = reg_v.at(op.get_nibble(2));
                            if (old_shift) {
                                vx = reg_v.at(op.get_nibble(1)); // optional
                            }
                            reg_v.at(0xf) = vx & 0x80;
                            vx <<= 1;
                            break;
                        }
                    }
                    break;
                }

                case (0x9): {
                    // std::cout << "skip if vx != vy\n";
                    if (reg_v.at(op.get_nibble(2)) == op.get_nibble(1)) {
                        pc += 2;
                    }
                    break;
                }

                case (0xa): {
                    // std::cout << "set index\n";
                    i = op.get_12bits();
                    break;
                }

                case (0xb): {
                    // std::cout << "jump with offset\n";
                    if (old_jump_offset) {
                        pc = op.get_12bits() + reg_v.at(0x0);
                    } else {
                        pc = op.get_12bits() + reg_v.at(op.get_nibble(2));
                    }
                    break;
                }

                case (0xc): {
                    // std::cout << "generate random number AND it with NN and set it to vx\n"
                    u8 rand_num = uni_dist(e1) & op.get_byte(0);
                    reg_v.at(op.get_nibble(2)) = rand_num;
                    break;
                }

                case (0xd): {
                    // std::cout << "drawing sprite\n";
                    auto x = reg_v.at(op.get_nibble(2));
                    auto y = reg_v.at(op.get_nibble(1));

                    auto sprite = memory.get_sprite(i, op.get_nibble(0));
                    bool has_flipped = display.draw_sprite(std::move(sprite), x, y);

                    reg_v.at(0x0f) = static_cast<u8>(has_flipped);
                    break;
                }

                case (0xe): {
                    switch (op.get_byte(0)) {
                        case (0x9e): {
                            // std::cout << "skip if key is pressed\n";
                            if (display.is_pressed(reg_v.at(op.get_nibble(2)))) {
                                // std::cout << "skiiiiiiiiip pressed\n";
                                pc += 2;
                            }
                            break;
                        }

                        case (0xa1): {
                            // std::cout << "skip if key is not pressed\n";
                            if (!display.is_pressed(reg_v.at(op.get_nibble(2)))) {
                                // std::cout << "skiiiiiiiiip not pressed\n";
                                pc += 2;
                            }
                            break;
                        }
                    }
                    break;
                }

                case (0xf): {
                    switch (op.get_byte(0)) {
                        case (0x07): {
                            reg_v.at(op.get_nibble(2)) = static_cast<u8>(timer);
                            break;
                        }

                        case (0x0a): {
                            // std::cout << "blocking...\n";
                            if (key_pressed == -1) {
                                for (u8 i = 0; i < 0xf; ++i) {
                                    if (display.is_pressed(i)) {
                                        key_pressed = static_cast<i8>(i);
                                        break;
                                    }
                                }
                            }

                            if (key_pressed != -1 && !display.is_pressed(key_pressed)) {
                                reg_v.at(op.get_nibble(2)) = key_pressed;
                                key_pressed = -1;
                            } else {
                                pc -= 2;
                            }
                            break;
                        }

                        case (0x15): {
                            timer = reg_v.at(op.get_nibble(2));
                            break;
                        }

                        case (0x18): {
                            sound = reg_v.at(op.get_nibble(2));
                            break;
                        }

                        case (0x1e): {
                            i += reg_v.at(op.get_nibble(2));
                            break;
                        }

                        case (0x29): {
                            i = memory.get_font_addr(reg_v.at(op.get_nibble(2)));
                            break;
                        }

                        case (0x33): {
                            // std::cout << "bcd\n";
                            memory.store_bcd(i, reg_v.at(op.get_nibble(2)));
                            break;
                        }

                        case (0x55): {
                            memory.store(i, op.get_nibble(2), gsl::make_span(reg_v));
                            break;
                        }

                        case (0x65): {
                            memory.load(i, op.get_nibble(2), gsl::make_span(reg_v));
                            break;
                        }
                    }
                    break;
                }

                default: {
                    std::cout << "unknown instruction\n";
                }
            }

            // NOLINTEND(*magic-numbers*)
            display.swap_buffers();
            display.poll_events();
        }
    }
}
