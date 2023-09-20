#include "cpu.h"

#include "commands.h"
#include "ram.h"

#include <limits>

namespace {
// instruction execution frequency
constexpr u16 instr_per_sec = 500;                    // frequency
constexpr double sec_per_instr = 1.0 / instr_per_sec; // period

} // namespace

Rng::Rng() : m_engine(m_rd()), m_dist(0, std::numeric_limits<u8>::max()) {}

unsigned int Rng::gen() {
    return m_dist(m_engine);
}

CPU::CPU(const std::filesystem::path &rom_file) : m_memory(rom_file), m_regs(rom_start), m_time_passed(0), m_key_pressed(-1) {}

void CPU::instr_cycle(const double dt) {
    m_time_passed += dt;
    m_regs.update_timers(dt);

    if (m_time_passed >= sec_per_instr) {
        m_time_passed -= sec_per_instr;

        // execute instructions
        fetch_decode_execute();

        m_display.swap_buffers();
        m_display.poll_events();
    }
}

void CPU::fetch_decode_execute() {
    auto op = m_memory.fetch(m_regs.get_pc());
    m_regs.incr_pc();

    // detect type of operation
    switch (op.get_operation_type()) {

        case (Operation::ClearReturn): {
            commands::clear_or_return(op, m_display, m_stack, m_regs);
            break;
        }

        case (Operation::Jump): {
            // std::cout << "jumping\n";
            commands::jump(op, m_regs);
            break;
        }

        case (Operation::Call): {
            // std::cout << "calling subroutine\n";
            commands::call(op, m_stack, m_regs);
            break;
        }

        case (Operation::IfRegNotEqualValue): {
            // std::cout << "skip if vx == NN\n";
            commands::if_reg_not_eq_value(op, m_regs);
            break;
        }

        case (Operation::IfRegEqualValue): {
            // std::cout << "skip if vx != NN\n";
            commands::if_reg_eq_value(op, m_regs);
            break;
        }

        case (Operation::IfRegNotEqualReg): {
            // std::cout << "skip if vx == vy\n";
            commands::if_reg_not_eq_reg(op, m_regs);
            break;
        }

        case (Operation::SetRegValue): {
            // std::cout << "set register\n";
            commands::set_reg_value(op, m_regs);
            break;
        }

        case (Operation::AddToReg): {
            // std::cout << "add NN to vx\n";
            commands::add_to_reg(op, m_regs);
            break;
        }

        case (Operation::RegOperations): {
            commands::reg_operations(op, m_regs);
            break;
        }

        case (Operation::IfRegEquality): {
            // std::cout << "skip if vx != vy\n";
            commands::if_reg_eq_reg(op, m_regs);
            break;
        }

        case (Operation::SetIndex): {
            // std::cout << "set index\n";
            commands::set_index(op, m_regs);
            break;
        }

        case (Operation::JumpV0Addr): {
            // std::cout << "jump with offset\n";
            commands::jump_add_plus_v0(op, m_regs);
            break;
        }

        case (Operation::RandomNumber): {
            // std::cout << "generate random number AND it with NN and set it to vx\n"
            commands::random_number(op, m_regs, m_rng);
            break;
        }

        case (Operation::LoadSprite): {
            // std::cout << "drawing sprite\n";
            commands::load_sprite(op, m_regs, m_memory, m_display);
            break;
        }

        case (Operation::KeyPress): {
            commands::key_press_operations(op, m_regs, m_display);
            break;
        }

        case (Operation::Other): {
            commands::other_operations(op, m_regs, m_memory, m_display, m_key_pressed);
            break;
        }

        default: {
            std::cout << "unknown instruction\n";
        }
    }
}
