#include <CLI/CLI.hpp>
#include <thread>

#include "chip8.h"
#include "sdl.h"
#include "timer.h"

int main(int argc, char** argv) {
  CLI::App app{"Chip8 emulator"};
  std::string path_to_game = "";
  app.add_option("-f,--file", path_to_game, "Game path.");

  CLI11_PARSE(app, argc, argv);

  auto game{load_game(path_to_game)};

  auto gfx{std::make_unique<SdlGfx>(1024, 512)};
  auto input{std::make_unique<SdlInput>()};
  Chip8 chip8{gfx.get(), input.get()};
  chip8.load(game);

  Timer timer_clock{std::chrono::milliseconds(1000 / 60), [&chip8]() { chip8.update_timers(); }};
  Timer cpu_clock{std::chrono::milliseconds(10), [&chip8]() { chip8.execute_cycle(); }};

  while (input->emulator_active()) {
  }
}