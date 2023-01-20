#include <gtest/gtest.h>

#include "chip8.h"
#include "sdl.h"

class OpCodeTest : public ::testing::Test {
 public:
  OpCodeTest() : gfx{}, in{std::array<bool, 16>{}}, c{Chip8{&gfx, &in}} {}

 protected:
  EmptyGfx gfx;
  MockedInput in;
  Chip8 c;
};

TEST_F(OpCodeTest, CLS_00E0) {
  gfx.set_pixel(10, 10, true);

  c.load({0x00, 0xE0});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(gfx.pixel(10, 10), false);
}

TEST_F(OpCodeTest, CALL_2xxx_RET_00EE) {
  std::vector<uint8_t> p(0xF, 0);
  p[0x0] = 0x22;
  p[0x1] = 0x06;
  p[0x6] = 0x00;
  p[0x7] = 0xEE;

  c.load(p);
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
  ASSERT_EQ(c.stack_pointer(), 1);
  ASSERT_EQ(c.stack(0), 0x0200);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.stack_pointer(), 0);
}

TEST_F(OpCodeTest, JMP_1xxx) {
  c.load({0x1A, 0xBC});
  c.execute_cycle();

  ASSERT_EQ(c.program_counter(), 0x0ABC);
}

TEST_F(OpCodeTest, SEVxNN_3xnn) {
  // LD VX,NN then SE Vx,NN
  // PC += 4
  c.load({0x65, 0xAB, 0x35, 0xAB});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);

  // PC += 2
  c.reset();
  c.load({0x65, 0xAB, 0x35, 0xFF});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
}

TEST_F(OpCodeTest, SNEVxNN_4xnn) {
  // LD VX,NN then SNE Vx,NN
  // PC += 2
  c.load({0x65, 0xAB, 0x45, 0xAB});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);

  // PC += 4
  c.reset();
  c.load({0x65, 0xAB, 0x45, 0xFF});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
}

TEST_F(OpCodeTest, SEVxVy_5xy0) {
  // LD Vy,NN then LD Vy,NN then SE Vx,Vy
  // PC += 4
  c.load({0x65, 0xAB, 0x67, 0xAB, 0x55, 0x70});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0x7), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x208);

  // PC += 2
  c.reset();
  c.load({0x65, 0xAB, 0x67, 0xFF, 0x55, 0x70});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0x7), 0xFF);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
}

TEST_F(OpCodeTest, LDVxNN_6xnn) {
  c.load({0x65, 0xAB});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);
}

TEST_F(OpCodeTest, ADDVxNN_7xkk) {
  // LD Vx,NN then ADD Vx,NN
  c.load({0x6D, 0x10, 0x7D, 0x20});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0xD), 0x10);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0xD), 0x10 + 0x20);
}

TEST_F(OpCodeTest, LDVxVy_8xy0) {
  // LD Vy,NN then LD Vx,Vy
  c.load({0x6D, 0xAB, 0x85, 0xD0});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0xD), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0x5), 0xAB);
}

TEST_F(OpCodeTest, DISABLED_ORVxVy_8xy1) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_ANDVxVy_8xy2) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_XORVxVy_8xy3) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_ADDVxVy_8xy4) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_SUBVxVy_8xy5) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_SHRVxVy_8xy6) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_SUBNVxVy_8xy7) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_SHLVxVy_8xyE) {
  // TODO
}

TEST_F(OpCodeTest, SNEVxVy_9xy0) {
  // LD Vy,NN then LD Vy,NN then SNE Vx,Vy
  // PC += 2
  c.load({0x65, 0xAB, 0x67, 0xAB, 0x95, 0x70});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0x7), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);

  // PC += 4
  c.reset();
  c.load({0x65, 0xAB, 0x67, 0xFF, 0x95, 0x70});
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xAB);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0x7), 0xFF);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x208);
}

TEST_F(OpCodeTest, LDIaddr_Annn) {
  c.load({0xAC, 0xDE});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.index_register(), 0xCDE);
}

TEST_F(OpCodeTest, JPV0addr_Bnnn) {
  // LD Vx,NN then JP V0,addr
  c.load({0x60, 0xAA, 0xBB, 0x88});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0), 0xAA);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0xB88 + 0xAA);
}

TEST_F(OpCodeTest, DISABLED_RNDVxNN_Cxnn) {
  // TODO: testing random
}

TEST_F(OpCodeTest, DISABLED_DRWVxVyn_Dxyn) {
  // TODO: testing graphics
}

TEST_F(OpCodeTest, SKP_Ex9E) {
  // Key not pressed, then pressed.
  in.set_key_state(0xA, true);
  c.load({0xE0, 0x9E, 0xEA, 0x9E});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
}

TEST_F(OpCodeTest, SKPN_ExA1) {
  // Key pressed, then not pressed.
  in.set_key_state(0xA, true);
  c.load({0xEA, 0xA1, 0xE0, 0xA1});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
}

TEST_F(OpCodeTest, LDVxDT_Fx07) {
  // LD Vx,NN then LD DT,Vx then LD Vx,DT
  c.load({0x65, 0xCC, 0xF5, 0x15, 0xF3, 0x07});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xCC);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.delay_timer(), 0xCC - 1);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
  ASSERT_EQ(c.registers(0x3), 0xCC - 1);
}

TEST_F(OpCodeTest, LDVxK_Fx0A) {
  c.load({0xFC, 0x0A});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x200);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x200);

  in.set_key_state(0xD, true);
  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0xC), 0xD);
}

TEST_F(OpCodeTest, LDDTVx_Fx15) {
  // LD Vx,NN then LD DT,Vx
  c.load({0x65, 0xCC, 0xF5, 0x15});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xCC);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.delay_timer(), 0xCC - 1);
}

TEST_F(OpCodeTest, LDSTVx_Fx18) {
  // LD Vx,NN then LD ST,Vx
  c.load({0x65, 0xCC, 0xF5, 0x18});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.registers(0x5), 0xCC);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.sound_timer(), 0xCC - 1);
}

TEST_F(OpCodeTest, ADDIVx_Fx1E) {
  // LD I,addr then LD Vx,NN then ADD I,Vx
  c.load({0xAC, 0xDE, 0x65, 0xFE, 0xF5, 0x1E});

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x202);
  ASSERT_EQ(c.index_register(), 0xCDE);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x204);
  ASSERT_EQ(c.registers(0x5), 0xFE);

  c.execute_cycle();
  ASSERT_EQ(c.program_counter(), 0x206);
  ASSERT_EQ(c.index_register(), 0xCDE + 0xFE);
}

TEST_F(OpCodeTest, DISABLED_LDFVx_Fx29) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_LDBVx_Fx33) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_LDIVx_Fx55) {
  // TODO
}

TEST_F(OpCodeTest, DISABLED_LDVxI_Fx65) {
  // TODO
}
