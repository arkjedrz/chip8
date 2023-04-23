#include <CLI/CLI.hpp>
#include <thread>

#include "chip8.h"
#include "sdl.h"
#include "timer.h"

int main(int argc, char** argv) {
  CLI::App app{"Chip8 emulator"};
  std::string path_to_game = "";
  app.add_option("-f,--file", path_to_game, "Game path.");
  int64_t interval = 5;
  app.add_option("-i,--interval", interval, "Interval between CPU cycles.");
  CLI11_PARSE(app, argc, argv);

  auto game{load_game(path_to_game)};

  SdlGfx gfx{1024, 512};
  SdlInput input;
  SdlAudio audio;
  Chip8 chip8{gfx, input, audio};
  chip8.load(game);

  Timer timer_clock{std::chrono::milliseconds(1000 / 60), [&chip8]() { chip8.update_timers(); }};
  Timer cpu_clock{std::chrono::milliseconds(interval), [&chip8]() { chip8.execute_cycle(); }};

  std::mutex exit_mutex;
  std::unique_lock<std::mutex> exit_lock{exit_mutex};

  input.emulator_active_cv().wait(exit_lock, [&input] { return !input.emulator_active(); });
}