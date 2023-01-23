#include "timer.h"

Timer::Timer(Interval interval, Callback callback)
    : interval_{interval}, callback_{callback}, running_{true}, thread_{[&]() {
        auto prev_time{std::chrono::system_clock::now()};
        while (running_) {
          auto until_time{prev_time + interval_};

          callback_();

          std::this_thread::sleep_until(until_time);
          prev_time = until_time;
        }
      }} {}

Timer::~Timer() {
  running_ = false;
  thread_.join();
}

void Timer::set_interval(Interval interval) { interval_ = interval; }