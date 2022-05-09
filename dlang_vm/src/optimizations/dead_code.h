// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <array>
#include <unordered_map>

#include "optimization.h"

class DeadCodeElimination : public OptimizationGraph {
 public:
  virtual void optimizeGraph(GraphPtr graph);
};
