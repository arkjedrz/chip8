#pragma once

#include <chrono>
#include <functional>
#include <thread>

class Timer {
 public:
  using Interval = std::chrono::milliseconds;
  using Callback = std::function<void()>;

  Timer(Interval interval, Callback callback);
  ~Timer();

  void set_interval(Interval new_interval);

 private:
  Interval interval_;
  Callback callback_;
  bool running_;
  std::thread thread_;
};