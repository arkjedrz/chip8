#include <gtest/gtest.h>

#include "hello.h"

TEST(Hello, Get) {
  auto value{get_hello("You")};
  ASSERT_STREQ(value.c_str(), "Hello You!");
}

TEST(Hello, Fail) { ASSERT_EQ(1, 2); }