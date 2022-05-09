// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <vector>

#include "optimization.h"

class OptimizationsSequence {
 public:
  void add(std::shared_ptr<Optimization> optimization);
  Code<UInstruction> optimizeFunction(const Code<UInstruction>& code);
  Code<UInstruction> optimizeTrace(const Code<UInstruction>& code);
 private:
  Code<UInstruction> optimize(const Code<UInstruction>& code,
                              std::shared_ptr<TState> tState);
  std::vector<std::shared_ptr<Optimization>> optimizations_;
};
