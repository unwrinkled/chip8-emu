#include <gtest/gtest.h>

#include <chip8/memory.h>

TEST(MemoryTests, OpcodeFetching) {
    Memory memory("roms/test_opcode.ch8");

    constexpr u16 pc = 512;
    Opcode opcode = memory.fetch(pc);
    EXPECT_EQ(opcode, 0x124e);
}
