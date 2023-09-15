#include <gtest/gtest.h>

#include <ram.h>

static constexpr u16 test_opcode = 0x1234;

TEST(MemoryTests, OpcodeNibbles) {
    Opcode op(test_opcode);

    EXPECT_EQ(op.get_nibble(0), 0x04);
    EXPECT_EQ(op.get_nibble(1), 0x03);
    EXPECT_EQ(op.get_nibble(2), 0x02);
    EXPECT_EQ(op.get_nibble(3), 0x01);
}

TEST(MemoryTests, OpcodeBytes) {
    Opcode op(test_opcode);

    EXPECT_EQ(op.get_byte(0), 0x34);
    EXPECT_EQ(op.get_byte(1), 0x12);
}

TEST(MemoryTests, Opcode12bits) {
    Opcode op(test_opcode);

    EXPECT_EQ(op.get_12bits(), 0x0234);
}

TEST(MemoryTests, OpcodeFetching) {
    RAM memory("roms/test_opcode.ch8");

    constexpr u16 pc = 512;
    Opcode opcode = memory.fetch(pc);
    EXPECT_EQ(opcode.get_nibble(0), 0x0e);
    EXPECT_EQ(opcode.get_nibble(1), 0x04);
    EXPECT_EQ(opcode.get_nibble(2), 0x02);
    EXPECT_EQ(opcode.get_nibble(3), 0x01);
}

TEST(MemoryTests, StoreToIndex) {
    RAM memory("roms/test_opcode.ch8");

    constexpr u16 i = 0x0;
    constexpr u8 vx = 123;
    memory.store_bcd(i, vx);
    EXPECT_EQ(memory.fetch_i(i), 1);
    EXPECT_EQ(memory.fetch_i(i + 1), 2);
    EXPECT_EQ(memory.fetch_i(i + 2), 3);
}
