#pragma once

#include <SDL2/SDL.h>

#include <condition_variable>
#include <memory>

class EmptyInput {
 public:
  // Get value. Map of keys.
  // 1 2 3 C
  // 4 5 6 D
  // 7 8 9 E
  // A 0 B F
  std::array<bool, 16> key_state();

  // Set state of a key.
  void set_key_state(int key, bool state);

 private:
  std::array<bool, 16> state_;
};

class SdlInput {
 public:
  // Get value. Map of keys.
  // 1 2 3 C
  // 4 5 6 D
  // 7 8 9 E
  // A 0 B F
  std::array<bool, 16> key_state();

  bool emulator_active() const;
  std::condition_variable& emulator_active_cv();

 private:
  bool emulator_active_{true};
  std::condition_variable cv_;
};

template <typename Impl>
class Gfx {
 public:
  Gfx() : map_{} {}

  // Set value of a pixel.
  void set_pixel(int x, int y, bool value) {
    // Prevent out-of-bounds write.
    auto pos{y * chip8_width + x};
    if (pos >= chip8_height * chip8_width) {
      return;
    }
    map_.at(pos) = value;
  }

  // Get value of a pixel.
  bool pixel(int x, int y) const {
    // Prevent out-of-bounds read.
    auto pos{y * chip8_width + x};
    if (pos >= chip8_height * chip8_width) {
      return false;
    }

    return map_.at(pos);
  }

  // Clear screen.
  void clear_screen() { map_ = {}; }

 protected:
  static const int chip8_width{64};
  static const int chip8_height{32};
  std::array<bool, chip8_width * chip8_height> map_;
};

class EmptyGfx : public Gfx<EmptyGfx> {
 public:
  // Do nothing.
  void render() {}
};

class SdlGfx : public Gfx<SdlGfx> {
 public:
  SdlGfx(int window_width, int window_height);
  ~SdlGfx();

  // Render to screen.
  void render();

 private:
  int width_{};
  int height_{};
  SDL_Window* window_{};
  SDL_Surface* surface_{};
};

class EmptyAudio {
 public:
  void play() {}
  void stop() {}
};

class SdlAudio {
 public:
  SdlAudio();
  ~SdlAudio();

  void play();
  void stop();

 private:
  SDL_AudioDeviceID device_{};
  SDL_AudioSpec spec_{};
  std::unique_ptr<int16_t[]> sample_{};
};