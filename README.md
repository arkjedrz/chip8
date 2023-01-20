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
