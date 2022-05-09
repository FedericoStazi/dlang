// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

class ExecutionStatistics {
 public:
  explicit ExecutionStatistics(size_t codeSize);

  void addCompiled(size_t cp, size_t size);

  void countRunJIT(size_t cp);
  void countInterpreted(size_t cp);

  size_t getCodeSize() const;
  size_t getLength(size_t cp) const;
  size_t getCompiledUsage(size_t cp) const;
  size_t getInterpretedCount() const;
  size_t getCompiledCount() const;

 private:
  size_t codeSize_;
  std::vector<size_t> length_;
  std::vector<size_t> compiledUsage_;
  size_t interpretedCount_ = 0;
  size_t compiledCount_ = 0;
};
