#pragma once

#include <SDL2/SDL.h>

#include <condition_variable>
#include <memory>

class Input {
 public:
  virtual ~Input() = default;

  // Get value. Map of keys.
  // 1 2 3 C
  // 4 5 6 D
  // 7 8 9 E
  // A 0 B F
  virtual std::array<bool, 16> key_state() = 0;
};

class MockedInput : public Input {
 public:
  ~MockedInput() = default;

  std::array<bool, 16> key_state() override;

  void set_key_state(int key, bool state);

 private:
  std::array<bool, 16> state_;
};

class SdlInput : public Input {
 public:
  ~SdlInput() = default;

  std::array<bool, 16> key_state() override;

  bool emulator_active() const;
  std::condition_variable& emulator_active_cv();

 private:
  bool emulator_active_{true};
  std::condition_variable cv_;
};

class Gfx {
 public:
  Gfx();
  virtual ~Gfx() = default;

  // Set value of a pixel.
  void set_pixel(int x, int y, bool value);

  // Get value of a pixel.
  bool pixel(int x, int y) const;

  // Clear screen.
  void clear_screen();

  // Render to screen.
  virtual void render() = 0;

 protected:
  static const int chip8_width{64};
  static const int chip8_height{32};
  std::array<bool, chip8_width * chip8_height> map_;
};

class EmptyGfx : public Gfx {
  void render() override {}
};

class SdlGfx : public Gfx {
 public:
  SdlGfx(int window_width, int window_height);
  ~SdlGfx() override;

  // Render to screen.
  void render() override;

 private:
  int width_{};
  int height_{};
  SDL_Window* window_{};
  SDL_Surface* surface_{};
};

class Audio {
 public:
  virtual ~Audio() = default;

  virtual void play() = 0;
  virtual void stop() = 0;
};

class EmptyAudio : public Audio {
 public:
  void play() override {}
  void stop() override {}
};

class SdlAudio : public Audio {
 public:
  SdlAudio();
  ~SdlAudio() override;

  void play() override;
  void stop() override;

 private:
  SDL_AudioDeviceID device_{};
  SDL_AudioSpec spec_{};
  std::unique_ptr<int16_t[]> sample_{};
};