// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "execution_statistics_out.h"

#include <string>

template<>
std::string Out::print(const ExecutionStatistics& statistics) {
  std::string str;
  str += printSpaced("JIT compilation statistics:") + "\n";
  str += printSpaced("Total number of interpreted instructions executed:",
                     statistics.getInterpretedCount()) + "\n";
  str += printSpaced("Total number of compiled instructions executed:",
                     statistics.getCompiledCount()) + "\n";
  str += printSpaced("Usage of compiled instructions:") + "\n";
  str += printRow(10, "", "cp", "length", "compiled") + "\n";
  for (int cp = 0; cp < statistics.getCodeSize(); cp++) {
    str += printRow(10, "",
                        cp,
                        statistics.getLength(cp),
                        statistics.getCompiledUsage(cp)) + "\n";
  }
  str += "\n";
  return str;
}
