#pragma once

#include <SDL2/SDL.h>

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

  // False, when SDL_QUIT detected.
  virtual bool emulator_active() const = 0;
};

class MockedInput : public Input {
 public:
  ~MockedInput() = default;

  std::array<bool, 16> key_state() override;
  bool emulator_active() const override;

  void set_key_state(int key, bool state);

 private:
  std::array<bool, 16> state_;
};

class SdlInput : public Input {
 public:
  ~SdlInput() = default;

  std::array<bool, 16> key_state() override;
  bool emulator_active() const override;

 private:
  bool emulator_active_{true};
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
  const int chip8_width_{64};
  const int chip8_height_{32};
  std::unique_ptr<bool[]> map_;
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