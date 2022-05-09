// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <chrono>

class Timer {
 public:
  void start();
  void stop();

  int64_t getDurationNS() const;

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
  int64_t duration_ = 0;
};
