#include <iostream>

#include "hello.h"

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  std::cout << "Hello world from main." << std::endl;
  std::cout << get_hello("You") << std::endl;
  return 0;
}