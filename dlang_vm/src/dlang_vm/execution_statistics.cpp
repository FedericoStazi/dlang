// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "execution_statistics.h"

#include <algorithm>

ExecutionStatistics::ExecutionStatistics(size_t codeSize)
    : codeSize_(codeSize), length_(codeSize), compiledUsage_(codeSize) {}

void ExecutionStatistics::addCompiled(size_t cp, size_t size) {
  length_[cp] = size;
}

void ExecutionStatistics::countRunJIT(size_t cp) {
  compiledCount_ += length_[cp];
  compiledUsage_[cp]++;
}

void ExecutionStatistics::countInterpreted(size_t cp) {
  interpretedCount_++;
}

size_t ExecutionStatistics::getCodeSize() const {
  return codeSize_;
}

size_t ExecutionStatistics::getLength(size_t cp) const {
  return length_[cp];
}

size_t ExecutionStatistics::getCompiledUsage(size_t cp) const {
  return compiledUsage_[cp];
}

size_t ExecutionStatistics::getInterpretedCount() const {
  return interpretedCount_;
}

size_t ExecutionStatistics::getCompiledCount() const {
  return compiledCount_;
}
