#pragma once

#include <cstdint>
#include <vector>

#include "game.h"
#include "sdl.h"

class Chip8 {
 public:
  Chip8(Gfx* const gfx, Input* const input);

  uint8_t ram(uint16_t index) const;
  uint8_t registers(uint8_t index) const;
  uint8_t delay_timer() const;
  uint8_t sound_timer() const;
  uint16_t index_register() const;
  uint16_t program_counter() const;
  uint8_t stack_pointer() const;
  uint16_t stack(uint8_t index) const;

  void load(const std::vector<uint8_t>& game);

  // Run one CPU cycle.
  void execute_cycle();

  // Update timers. Should be invoked by independent clock.
  void update_timers();

 private:
  Gfx* gfx_;
  Input* input_;

  std::array<uint8_t, 0x1000> ram_;
  std::array<uint8_t, 16> registers_;
  uint8_t dt_;
  uint8_t st_;
  uint16_t ir_;
  uint16_t pc_;
  uint8_t sp_;
  std::array<uint16_t, 16> stack_;

  void print_status(uint16_t current_opcode) const;
};