#include "sdl.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <vector>

std::array<bool, 16> EmptyInput::key_state() { return state_; }

void EmptyInput::set_key_state(int key, bool state) { state_.at(key) = state; }

std::array<bool, 16> SdlInput::key_state() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      emulator_active_ = false;
      cv_.notify_all();
    }
  }

  SDL_PumpEvents();

  int len_state{0};
  const Uint8* state_raw = SDL_GetKeyboardState(&len_state);
  // Ignoring due to SDL interface.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  std::vector<uint8_t> state{state_raw, state_raw + len_state};

  std::array<bool, 16> key_state{};

  if (state.at(SDL_SCANCODE_1)) {
    key_state[0x1] = true;
  }
  if (state.at(SDL_SCANCODE_2)) {
    key_state[0x2] = true;
  }
  if (state.at(SDL_SCANCODE_3)) {
    key_state[0x3] = true;
  }
  if (state.at(SDL_SCANCODE_4)) {
    key_state[0xC] = true;
  }

  if (state.at(SDL_SCANCODE_Q)) {
    key_state[0x4] = true;
  }
  if (state.at(SDL_SCANCODE_W)) {
    key_state[0x5] = true;
  }
  if (state.at(SDL_SCANCODE_E)) {
    key_state[0x6] = true;
  }
  if (state.at(SDL_SCANCODE_R)) {
    key_state[0xD] = true;
  }

  if (state.at(SDL_SCANCODE_A)) {
    key_state[0x7] = true;
  }
  if (state.at(SDL_SCANCODE_S)) {
    key_state[0x8] = true;
  }
  if (state.at(SDL_SCANCODE_D)) {
    key_state[0x9] = true;
  }
  if (state.at(SDL_SCANCODE_F)) {
    key_state[0xE] = true;
  }

  if (state.at(SDL_SCANCODE_Z)) {
    key_state[0xA] = true;
  }
  if (state.at(SDL_SCANCODE_X)) {
    key_state[0x0] = true;
  }
  if (state.at(SDL_SCANCODE_C)) {
    key_state[0xB] = true;
  }
  if (state.at(SDL_SCANCODE_V)) {
    key_state[0xF] = true;
  }

  return key_state;
}

bool SdlInput::emulator_active() const { return emulator_active_; }

std::condition_variable& SdlInput::emulator_active_cv() { return cv_; }

SdlGfx::SdlGfx(int window_width, int window_height)
    : Gfx{},
      width_{window_width},
      height_{window_height}

{
  SDL_Init(SDL_INIT_VIDEO);
  std::string title{"Chip8"};
  window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_,
                             SDL_WINDOW_SHOWN);
  // Ignoring due to SDL interface.
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  surface_ = SDL_GetWindowSurface(window_);
}

SdlGfx::~SdlGfx() {
  SDL_FreeSurface(surface_);
  SDL_DestroyWindow(window_);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

// Render to screen.
void SdlGfx::render() {
  int block_width{width_ / chip8_width};
  int block_height{height_ / chip8_height};
  for (int y = 0; y < chip8_height; ++y) {
    for (int x = 0; x < chip8_width; ++x) {
      SDL_Rect block{x * block_width, y * block_height, block_width, block_height};

      auto color_v{map_.at(y * chip8_width + x) ? 0xFF : 0};
      Uint8 color{static_cast<Uint8>(color_v)};
      Uint32 color_sdl{SDL_MapRGB(surface_->format, color, color, color)};

      SDL_FillRect(surface_, &block, color_sdl);
    }
  }

  SDL_UpdateWindowSurface(window_);
}

SdlAudio::SdlAudio() {
  SDL_Init(SDL_INIT_AUDIO);

  spec_.freq = 44100;
  spec_.format = AUDIO_S16SYS;
  spec_.channels = 1;
  spec_.samples = 1024;
  spec_.callback = nullptr;

  device_ = SDL_OpenAudioDevice(nullptr, 0, &spec_, nullptr, 0);

  // Prepare 1 second sample.
  sample_ = std::make_unique<int16_t[]>(spec_.freq);
  float x = 0.0f;
  for (int i = 0; i < spec_.freq; ++i) {
    x += .01f;
    sample_[i] = sin(x * 4) * 5000;
  }

  // Load sample to device.
  const auto sample_size{spec_.freq * sizeof(int16_t)};
  SDL_QueueAudio(device_, sample_.get(), sample_size);
}

SdlAudio::~SdlAudio() {
  SDL_CloseAudioDevice(device_);
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SdlAudio::play() { SDL_PauseAudioDevice(device_, 0); }

void SdlAudio::stop() { SDL_PauseAudioDevice(device_, 1); }