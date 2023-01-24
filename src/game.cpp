#include "game.h"

#include <filesystem>
#include <fstream>

std::vector<uint8_t> load_game(const std::string& file_path) {
  using namespace std::filesystem;

  path path{file_path};
  exists(path);

  std::error_code ec;
  if (!exists(path, ec)) {
    throw filesystem_error("File not found.", path, ec);
  }

  size_t size{std::filesystem::file_size(path)};
  std::vector<uint8_t> data(size, 0);
  std::ifstream rom_input(path.c_str(), std::ios::in | std::ios::binary);
  // Ignoring due to no other way of doing it.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  rom_input.readsome(reinterpret_cast<char*>(&data[0]), static_cast<std::streamsize>(size));

  return data;
}