// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <unordered_map>
#include <unordered_set>

#include "optimization.h"

class RemoveUnusedWrites : public OptimizationGraph {
 protected:
  void optimizeGraph(GraphPtr graph);
};
