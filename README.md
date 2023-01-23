# Chip8

Basic Chip8 emulator written in C++.

## Usage

Quick start:

```bash
mkdir build && cd build
conan install .. --build=missing -o testing=True
cmake -D TESTING=ON ..
cmake --build .
./bin/Chip8
```

## Tested configurations

- Ubuntu 22.04

## TODOs

- Add sound
- Fix all Chip-8 quirks from test suite

## Acknowlegments

- [Cowgod's Chip-8 Technical Reference](devernay.free.fr/hacks/chip8/)
- [cookerlyk/Chip8](https://github.com/cookerlyk/Chip8)
- [Timendus/chip8-test-suite](https://github.com/Timendus/chip8-test-suite)