#include "hello.h"

std::string get_hello(const std::string& name) {
  return std::string("Hello ") + name + "!";
}