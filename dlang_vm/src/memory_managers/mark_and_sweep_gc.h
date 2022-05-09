// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include "no_allocation.h"

class MarkAndSweepGC : public NoAllocation {
 public:
  explicit MarkAndSweepGC(size_t initialSize = 1'000);

  virtual void collectGarbage(std::shared_ptr<VirtualMachine> vm);

 private:
  void markRecursive(size_t idx, std::shared_ptr<VirtualMachine> vm);

  std::set<size_t> marked_;
  std::unordered_map<size_t, size_t> newIndex_;
};
