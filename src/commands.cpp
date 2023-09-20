#include "commands.h"
#include "ram.h"
#include <limits>

namespace commands {

// chip8 version switches
inline constexpr bool old_shift = false;
inline constexpr bool old_jump_offset = true;

inline constexpr u8 flag_register = 0xf;
inline constexpr u8 most_significant_mask = 0x80;
inline constexpr u8 most_significant_shift = 7;

void clear_or_return(const Opcode &op, Display &display, std::stack<u16> &stack, Registers &regs) {
    switch (static_cast<ClearReturn>(op.get_12bits())) {
        case (ClearReturn::Clear): {
            display.clear_screen();
            break;
        }
        case (ClearReturn::Return): {
            if (stack.empty()) {
                throw std::runtime_error("trying to pop address from stack while empty");
            }
            regs.set_pc(stack.top());
            stack.pop();
            break;
        }
        default: {
            std::cout << "skip instruction\n";
        }
    }
}

void reg_operations(const Opcode &op, Registers &regs) noexcept {
    switch (static_cast<RegOperation>(op.get_nibble(0))) {

        case (RegOperation::Set): {
            // std::cout << "set vx = vy\n";
            regs.at(op.get_nibble(2)) = regs.at(op.get_nibble(1));
            break;
        }

        case (RegOperation::BitwiseOr): {
            // std::cout << "logical or\n";
            regs.at(op.get_nibble(2)) |= regs.at(op.get_nibble(1));
            regs.at(flag_register) = 0;
            break;
        }

        case (RegOperation::BitwiseAnd): {
            // std::cout << "logical and\n";
            regs.at(op.get_nibble(2)) &= regs.at(op.get_nibble(1));
            regs.at(flag_register) = 0;
            break;
        }

        case (RegOperation::BitwiseXor): {
            // std::cout << "logical xor\n";
            regs.at(op.get_nibble(2)) ^= regs.at(op.get_nibble(1));
            regs.at(flag_register) = 0;
            break;
        }

        case (RegOperation::Addition): {
            // std::cout << "add vy to vx and update carry flag\n";
            auto &vx = regs.at(op.get_nibble(2));
            auto vy = regs.at(op.get_nibble(1));
            auto flag = std::numeric_limits<u8>::max() - vx < vy ? 1 : 0;

            vx += vy;
            regs.at(flag_register) = flag;
            break;
        }

        case (RegOperation::Subtraction): {
            // std::cout << "subtract vy from vx and update borrow flag\n";
            auto &vx = regs.at(op.get_nibble(2));
            auto vy = regs.at(op.get_nibble(1));
            auto flag = vx >= vy ? 1 : 0;

            vx -= vy;
            regs.at(flag_register) = flag;
            break;
        }

        case (RegOperation::ShiftRight): {
            // std::cout << "vx >> 1 and set vf to old shifted bit\n";
            auto &vx = regs.at(op.get_nibble(2));
            if (old_shift) {
                vx = regs.at(op.get_nibble(1)); // optional
            }
            auto flag = vx & 0x1;

            vx >>= 1;
            regs.at(flag_register) = flag;
            break;
        }

        case (RegOperation::AltSubtraction): {
            // std::cout << "set vx = vy - vx and update borrow flag\n";
            auto &vx = regs.at(op.get_nibble(2));
            auto vy = regs.at(op.get_nibble(1));
            auto flag = vy >= vx ? 1 : 0;

            vx = vy - vx;
            regs.at(flag_register) = flag;
            break;
        }

        case (RegOperation::ShiftLeft): {
            // std::cout << "vx << 1 and set vf to old shifted bit\n";
            auto &vx = regs.at(op.get_nibble(2));
            if (old_shift) {
                vx = regs.at(op.get_nibble(1)); // optional
            }
            auto flag = (vx & most_significant_mask) >> most_significant_shift;

            vx <<= 1;
            regs.at(flag_register) = flag;
            break;
        }
    }
}

void jump_add_plus_v0(const Opcode &op, Registers &regs) noexcept {
    if (old_jump_offset) {
        regs.set_pc(op.get_12bits() + regs.at(0x0));
    } else {
        regs.set_pc(op.get_12bits() + regs.at(op.get_nibble(2)));
    }
}

void load_sprite(const Opcode &op, Registers &regs, RAM<> &memory, Display &display) noexcept {
    auto x = regs.at(op.get_nibble(2));
    auto y = regs.at(op.get_nibble(1));

    auto sprite = memory.get_sprite(regs.get_index(), op.get_nibble(0));
    bool has_flipped = display.draw_sprite(std::move(sprite), x, y);

    regs.at(flag_register) = static_cast<u8>(has_flipped);
}

void key_press_operations(const Opcode &op, Registers &regs, Display &display) noexcept {
    switch (static_cast<KeyPressOp>(op.get_byte(0))) {
        case (KeyPressOp::IfKeyNotPressed): {
            // std::cout << "skip if key is pressed\n";
            if (display.is_pressed(regs.at(op.get_nibble(2)))) {
                // std::cout << "skiiiiiiiiip pressed\n";
                regs.incr_pc();
            }
            break;
        }

        case (KeyPressOp::IfKeyPressed): {
            // std::cout << "skip if key is not pressed\n";
            if (!display.is_pressed(regs.at(op.get_nibble(2)))) {
                // std::cout << "skiiiiiiiiip not pressed\n";
                regs.incr_pc();
            }
            break;
        }
    }
}

void other_operations(const Opcode &op, Registers &regs, RAM<> &memory, Display &display, i8 &key_pressed) noexcept {
    constexpr u8 total_keys = 16;
    switch (static_cast<OtherOp>(op.get_byte(0))) {
        case (OtherOp::SetVxDelay): {
            regs.at(op.get_nibble(2)) = static_cast<u8>(regs.get_timer());
            break;
        }

        case (OtherOp::SetVxKey): {
            // std::cout << "blocking...\n";
            if (key_pressed == -1) {
                for (u8 k = 0; k < total_keys; ++k) {
                    if (display.is_pressed(k)) {
                        key_pressed = static_cast<i8>(k);
                        break;
                    }
                }
                if (key_pressed != -1) {
                    regs.at(op.get_nibble(2)) = key_pressed;
                }
                regs.decr_pc();
            } else if (display.is_pressed(key_pressed)) {
                regs.decr_pc();
            } else {
                key_pressed = -1;
            }

            break;
        }

        case (OtherOp::SetDelay): {
            regs.set_timer(regs.at(op.get_nibble(2)));
            break;
        }

        case (OtherOp::SetBuzzer): {
            regs.set_sound(regs.at(op.get_nibble(2)));
            break;
        }

        case (OtherOp::AddVxToIndex): {
            regs.add_index(regs.at(op.get_nibble(2)));
            break;
        }

        case (OtherOp::SetIndexToHex): {
            regs.set_index(memory.get_font_addr(regs.at(op.get_nibble(2))));
            break;
        }

        case (OtherOp::BcdVx): {
            // std::cout << "bcd\n";
            memory.store_bcd(regs.get_index(), regs.at(op.get_nibble(2)));
            break;
        }

        case (OtherOp::StoreToRam): {
            memory.store(regs.get_index(), op.get_nibble(2), regs.get_regs_span());
            break;
        }

        case (OtherOp::LoadFromRam): {
            memory.load(regs.get_index(), op.get_nibble(2), regs.get_regs_span());
            break;
        }
    }
}

} // namespace commands
