// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "timer_out.h"

#include <string>

template<>
std::string Out::print(const Timer& timer) {
  auto duration = timer.getDurationNS();
  return printRow(4, "Time:  ",
                     ((duration / 1'000'000'000)), "s",
                     ((duration / 1'000'000) % 1'000), "ms",
                     ((duration / 1'000) % 1'000), "us",
                     ((duration) % 1'000), "ns") + "\n";
}
