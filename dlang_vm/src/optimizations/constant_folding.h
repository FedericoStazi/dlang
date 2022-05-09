// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "optimization.h"

class ConstantFolding : public OptimizationGraph {
 public:
  virtual void optimizeGraph(GraphPtr graph);
};
