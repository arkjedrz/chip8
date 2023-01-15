# cpp_template

CMake/Conan-based template for C++ projects.
Tailored for personal usage.

## Usage

Replace cpp_template with project name and remove redundant example files.

Quick start:

```bash
mkdir build && cd build
conan install .. --build=missing -o testing=True
cmake -D TESTING=ON ..
cmake --build .
./bin/cpp_template
```

## Tested configurations

- Ubuntu 22.04
