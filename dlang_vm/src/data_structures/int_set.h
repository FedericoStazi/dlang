// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <cstddef>
#include <vector>

class IntSet {
 public:
  void insert(const IntSet& other);
  void insert(size_t x);
  void erase(size_t x);
  bool get(size_t x) const;
  bool operator!=(const IntSet& other) const;
 private:
  static constexpr size_t kNumBits = 8 * sizeof(size_t);
  std::vector<size_t> set_{32, 0};
};
