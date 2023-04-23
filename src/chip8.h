#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "fonts.h"
#include "game.h"
#include "random.h"
#include "sdl.h"

template <typename Gfx, typename Input, typename Audio>
class Chip8 {
 public:
  Chip8(Gfx& gfx, Input& input, Audio& audio)
      : gfx_{gfx},
        input_{input},
        audio_{audio},
        ram_{},
        registers_{},
        dt_{0},
        st_{0},
        ir_{0},
        pc_{0x200},
        sp_{0},
        stack_{}

  {
    std::copy(fonts.begin(), fonts.end(), ram_.begin());
  }

  Chip8(const Chip8& other) : Chip8(other) {}

  Chip8& operator=(const Chip8& other) {
    if (this == &other) {
      return *this;
    }

    gfx_ = other.gfx_;
    input_ = other.input_;
    audio_ = other.audio_;
    ram_ = other.ram_;
    registers_ = other.registers_;
    dt_ = other.dt_;
    st_ = other.st_;
    ir_ = other.ir_;
    pc_ = other.pc_;
    sp_ = other.sp_;
    stack_ = other.stack_;
    return *this;
  }

  uint8_t ram(uint16_t index) const { return ram_.at(index); }

  uint8_t registers(uint8_t index) const { return registers_.at(index); }

  uint8_t delay_timer() const { return dt_; }

  uint8_t sound_timer() const { return st_; }

  uint16_t index_register() const { return ir_; }

  uint16_t program_counter() const { return pc_; }

  uint8_t stack_pointer() const { return sp_; }

  uint16_t stack(uint8_t index) const { return stack_.at(index); }

  void load(const std::vector<uint8_t>& game) {
    const auto pc_offset{0x200};
    std::copy(game.begin(), game.end(), ram_.begin() + pc_offset);
  }

  // Run one CPU cycle.
  void execute_cycle() {
    auto key_state{input_.key_state()};
    auto inst_1{ram_.at(pc_)};
    auto inst_2{ram_.at(pc_ + 1)};
    auto opcode{static_cast<uint16_t>(inst_1 << 8 | inst_2)};

    switch (opcode & 0xF000) {
      // CLS and RET
      case 0x0000: {
        // CLS
        if ((opcode & 0x00FF) == 0x00E0) {
          gfx_.clear_screen();
          pc_ += 2;
          break;
        }
        // RET
        else if ((opcode & 0x00FF) == 0x00EE) {
          --sp_;
          pc_ = stack_.at(sp_);
          pc_ += 2;
          break;
        } else {
          throw std::runtime_error("Unknown opcode.");
        }
      }
      // JMP
      case 0x1000: {
        auto address{opcode & 0xFFF};
        pc_ = address;
        break;
      }
      // CALL
      case 0x2000: {
        auto address{opcode & 0xFFF};
        stack_.at(sp_) = pc_;
        ++sp_;
        pc_ = address;

        break;
      }
      // SE VX,NN
      case 0x3000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto value{opcode & 0x00FF};

        if (registers_.at(reg_x) == value) {
          pc_ += 4;
        } else {
          pc_ += 2;
        }
        break;
      }
      // SNE VX,NN
      case 0x4000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto value{opcode & 0x00FF};

        if (registers_.at(reg_x) != value) {
          pc_ += 4;
        } else {
          pc_ += 2;
        }
        break;
      }
      // SE VX,VY
      case 0x5000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto reg_x_value{registers_.at(reg_x)};
        auto reg_y{(opcode & 0x00F0) >> 4};
        auto reg_y_value{registers_.at(reg_y)};

        if (reg_x_value == reg_y_value) {
          pc_ += 4;
        } else {
          pc_ += 2;
        }
        break;
      }
      // LD Vx,NN
      case 0x6000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto value{opcode & 0x00FF};
        registers_.at(reg_x) = value;
        pc_ += 2;

        break;
      }
      // ADD Vx,NN
      case 0x7000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto value{opcode & 0x00FF};
        registers_.at(reg_x) += value;
        pc_ += 2;

        break;
      }
      //
      case 0x8000: {
        switch (opcode & 0x000F) {
          // LD Vx,Vy
          case 0x0000: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            registers_.at(reg_x) = registers_.at(reg_y);
            pc_ += 2;

            break;
          }
          // OR Vx,Vy
          case 0x0001: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            registers_.at(reg_x) |= registers_.at(reg_y);
            registers_.at(0xF) = 0;
            pc_ += 2;

            break;
          }
          // AND Vx,Vy
          case 0x0002: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            registers_.at(reg_x) &= registers_.at(reg_y);
            registers_.at(0xF) = 0;
            pc_ += 2;

            break;
          }
          // XOR Vx,Vy
          case 0x0003: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            registers_.at(reg_x) ^= registers_.at(reg_y);
            registers_.at(0xF) = 0;
            pc_ += 2;

            break;
          }
          // ADD Vx,Vy
          case 0x0004: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            auto res{registers_.at(reg_x) + registers_.at(reg_y)};

            registers_.at(reg_x) = res;
            registers_.at(0xF) = res > 255 ? 1 : 0;

            pc_ += 2;

            break;
          }
          // SUB Vx,Vy
          case 0x0005: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            auto res{registers_.at(reg_x) - registers_.at(reg_y)};

            auto cmp{registers_.at(reg_x) > registers_.at(reg_y)};
            registers_.at(reg_x) = res;
            registers_.at(0xF) = cmp ? 1 : 0;

            pc_ += 2;

            break;
          }
          // SHR Vx,[Vy]
          case 0x0006: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};

            auto lsb{registers_.at(reg_y) & 0b00000001};
            registers_.at(reg_x) = registers_.at(reg_y) >> 1;
            registers_.at(0xF) = lsb;

            pc_ += 2;

            break;
          }
          // SUBN Vx,Vy
          case 0x0007: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};
            auto res{registers_.at(reg_y) - registers_.at(reg_x)};

            auto cmp{registers_.at(reg_y) > registers_.at(reg_x)};
            registers_.at(reg_x) = res;
            registers_.at(0xF) = cmp ? 1 : 0;

            pc_ += 2;

            break;
          }
          // SHL Vx,[Vy]
          case 0x000E: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            auto reg_y{(opcode & 0x00F0) >> 4};

            auto msb{registers_.at(reg_y) & 0b10000000};
            registers_.at(reg_x) = registers_.at(reg_y) << 1;
            registers_.at(0xF) = msb > 0 ? 1 : 0;

            pc_ += 2;
            break;
          }

          default: {
            throw std::runtime_error("Unknown opcode.");
          }
        }
        break;
      }
      // SNE Vx,Vy
      case 0x9000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto reg_x_value{registers_.at(reg_x)};
        auto reg_y{(opcode & 0x00F0) >> 4};
        auto reg_y_value{registers_.at(reg_y)};

        if (reg_x_value != reg_y_value) {
          pc_ += 4;
        } else {
          pc_ += 2;
        }
        break;
      }
      // LD I,addr
      case 0xA000: {
        auto value{opcode & 0x0FFF};
        ir_ = value;
        pc_ += 2;
        break;
      }
      // JP V0,addr
      case 0xB000: {
        auto value{opcode & 0x0FFF};
        pc_ = value + registers_.at(0);

        break;
      }
      // RND Vx,nn
      case 0xC000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto value{opcode & 0x00FF};
        registers_.at(reg_x) = random(0, 0xFF) & value;
        pc_ += 2;
        break;
      }
      // DRW Vx,Vy,n
      case 0xD000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        auto reg_y{(opcode & 0x00F0) >> 4};

        auto n{opcode & 0x000F};

        registers_.at(0xF) = 0;
        for (int y = 0; y < n; ++y) {
          auto line{ram_.at(ir_ + y)};
          for (int x = 0; x < 8; ++x) {
            auto pos_x{registers_.at(reg_x)};
            auto pos_y{registers_.at(reg_y)};
            if ((line & (0x80 >> x)) != 0) {
              bool current_state{gfx_.pixel(pos_x + x, pos_y + y)};
              if (current_state) {
                registers_.at(0xF) = 1;
              }

              gfx_.set_pixel(pos_x + x, pos_y + y, !current_state != false);
            }
          }
        }

        pc_ += 2;
        break;
      }
      // SKP and SKNP
      case 0xE000: {
        auto reg_x{(opcode & 0x0F00) >> 8};
        switch (opcode & 0x00FF) {
          case 0x009E: {
            if (key_state.at(registers_.at(reg_x))) {
              pc_ += 2;
            }
            break;
          }
          case 0x00A1: {
            if (!key_state.at(registers_.at(reg_x))) {
              pc_ += 2;
            }
            break;
          }
          default: {
            throw std::runtime_error("Unknown opcode.");
          }
        }

        pc_ += 2;
        break;
      }
      case 0xF000: {
        switch (opcode & 0x00FF) {
          // LD Vx,DT
          case 0x0007: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            registers_.at(reg_x) = dt_;
            pc_ += 2;
            break;
          }
          // LD Vx,K
          case 0x000A: {
            auto key_pressed{std::any_of(key_state.begin(), key_state.end(), [](bool value) { return value; })};
            if (!key_pressed) {
              break;
            }

            auto reg_x{(opcode & 0x0F00) >> 8};
            auto key_index{std::find(key_state.begin(), key_state.end(), true) - key_state.begin()};
            registers_.at(reg_x) = key_index;

            pc_ += 2;
            break;
          }
          // LD DT,Vx
          case 0x0015: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            dt_ = registers_.at(reg_x);
            pc_ += 2;
            break;
          }
          // LD ST,Vx
          case 0x0018: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            st_ = registers_.at(reg_x);
            pc_ += 2;
            // Start playing sound.
            if (st_ > 0) {
              audio_.play();
            }

            break;
          }
          // ADD I,Vx
          case 0x001E: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            ir_ += registers_.at(reg_x);
            pc_ += 2;
            break;
          }
          // LD F, Vx
          case 0x0029: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            ir_ = registers_.at(reg_x) * 0x5;
            pc_ += 2;
            break;
          }
          // LD B, Vx
          case 0x0033: {
            auto reg_x{(opcode & 0x0F00) >> 8};

            ram_.at(ir_) = registers_.at(reg_x) / 100;
            ram_.at(ir_ + 1) = (registers_.at(reg_x) / 10) % 10;
            ram_.at(ir_ + 2) = (registers_.at(reg_x) % 100) % 10;

            pc_ += 2;
            break;
          }
          // LD [I],Vx
          case 0x0055: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            for (int i = 0; i <= reg_x; ++i) {
              ram_.at(ir_) = registers_.at(i);
              ir_ += 1;
            }

            pc_ += 2;
            break;
          }
          // LD Vx,[I]
          case 0x0065: {
            auto reg_x{(opcode & 0x0F00) >> 8};
            for (int i = 0; i <= reg_x; ++i) {
              registers_.at(i) = ram_.at(ir_);
              ir_ += 1;
            }

            pc_ += 2;
            break;
          }

          default: {
            throw std::runtime_error("Unknown opcode.");
          }
        }
        break;
      }

      default: {
        throw std::runtime_error("Unknown opcode.");
      }
    }

    gfx_.render();
  }

  // Update timers. Should be invoked by independent clock.
  void update_timers() {
    if (dt_ > 0) {
      --dt_;
    }
    if (st_ > 0) {
      --st_;
    } else {
      // Stop playing sound.
      audio_.stop();
    }
  }

 private:
  Gfx& gfx_;
  Input& input_;
  Audio& audio_;

  std::array<uint8_t, 0x1000> ram_;
  std::array<uint8_t, 16> registers_;
  uint8_t dt_;
  uint8_t st_;
  uint16_t ir_;
  uint16_t pc_;
  uint8_t sp_;
  std::array<uint16_t, 16> stack_;

  void print_status(uint16_t current_opcode) const {
    std::cout << "CURRENT OPCODE: " << std::hex << current_opcode << std::endl;
    std::cout << "registers_" << std::endl;
    for (int i = 0; i < 16; ++i) {
      std::cout << i << ": " << (int)registers_.at(i) << std::endl;
    }
    std::cout << "DT: " << (int)dt_ << std::endl;
    std::cout << "ST: " << (int)st_ << std::endl;
    std::cout << "I: " << (int)ir_ << std::endl;
    std::cout << "PC: " << (int)pc_ << std::endl;
    std::cout << "SP: " << (int)sp_ << std::endl;
    std::cout << "stack_" << std::endl;
    for (int i = 0; i < 16; ++i) {
      std::cout << i << ": " << (int)stack_.at(i) << std::endl;
    }
    std::cout << std::endl;
  }
};