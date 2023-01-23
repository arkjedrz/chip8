#include "sdl.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <string>

std::array<bool, 16> MockedInput::key_state() { return state_; }

bool MockedInput::emulator_active() const { return true; }

void MockedInput::set_key_state(int key, bool state) { state_[key] = state; }

std::array<bool, 16> SdlInput::key_state() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      emulator_active_ = false;
    }
  }

  SDL_PumpEvents();

  const Uint8* state = SDL_GetKeyboardState(nullptr);

  std::array<bool, 16> key_state{};

  if (state[SDL_SCANCODE_1]) {
    key_state[0x1] = true;
  }
  if (state[SDL_SCANCODE_2]) {
    key_state[0x2] = true;
  }
  if (state[SDL_SCANCODE_3]) {
    key_state[0x3] = true;
  }
  if (state[SDL_SCANCODE_4]) {
    key_state[0xC] = true;
  }

  if (state[SDL_SCANCODE_Q]) {
    key_state[0x4] = true;
  }
  if (state[SDL_SCANCODE_W]) {
    key_state[0x5] = true;
  }
  if (state[SDL_SCANCODE_E]) {
    key_state[0x6] = true;
  }
  if (state[SDL_SCANCODE_R]) {
    key_state[0xD] = true;
  }

  if (state[SDL_SCANCODE_A]) {
    key_state[0x7] = true;
  }
  if (state[SDL_SCANCODE_S]) {
    key_state[0x8] = true;
  }
  if (state[SDL_SCANCODE_D]) {
    key_state[0x9] = true;
  }
  if (state[SDL_SCANCODE_F]) {
    key_state[0xE] = true;
  }

  if (state[SDL_SCANCODE_Z]) {
    key_state[0xA] = true;
  }
  if (state[SDL_SCANCODE_X]) {
    key_state[0x0] = true;
  }
  if (state[SDL_SCANCODE_C]) {
    key_state[0xB] = true;
  }
  if (state[SDL_SCANCODE_V]) {
    key_state[0xF] = true;
  }

  return key_state;
}

bool SdlInput::emulator_active() const { return emulator_active_; }

Gfx::Gfx() { map_ = std::make_unique<bool[]>(chip8_height_ * chip8_width_); }

// Set value of a pixel.
void Gfx::set_pixel(int x, int y, bool value) { map_[y * chip8_width_ + x] = value; }

// Get value of a pixel.
bool Gfx::pixel(int x, int y) const { return map_[y * chip8_width_ + x]; }

// Clear screen.
void Gfx::clear_screen() { map_ = std::make_unique<bool[]>(chip8_height_ * chip8_width_); }

SdlGfx::SdlGfx(int window_width, int window_height)
    : Gfx{},
      width_{window_width},
      height_{window_height}

{
  SDL_Init(SDL_INIT_VIDEO);
  std::string title{"Chip8"};
  window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_,
                             SDL_WINDOW_SHOWN);
  surface_ = SDL_GetWindowSurface(window_);
}

SdlGfx::~SdlGfx() {
  SDL_FreeSurface(surface_);
  SDL_DestroyWindow(window_);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

// Render to screen.
void SdlGfx::render() {
  int block_width{width_ / chip8_width_};
  int block_height{height_ / chip8_height_};
  for (int y = 0; y < chip8_height_; ++y) {
    for (int x = 0; x < chip8_width_; ++x) {
      SDL_Rect block{x * block_width, y * block_height, block_width, block_height};

      auto color_v{map_[y * chip8_width_ + x] ? 0xFF : 0};
      Uint8 color{static_cast<Uint8>(color_v)};
      Uint32 color_sdl{SDL_MapRGB(surface_->format, color, color, color)};

      SDL_FillRect(surface_, &block, color_sdl);
    }
  }

  SDL_UpdateWindowSurface(window_);
}
