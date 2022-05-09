// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "timer.h"

void Timer::start() {
  startTime_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
  auto endTime = std::chrono::high_resolution_clock::now();
  duration_ = std::chrono::duration_cast<std::chrono::nanoseconds>
              (endTime - startTime_).count();
}

int64_t Timer::getDurationNS() const {
  return duration_;
}
