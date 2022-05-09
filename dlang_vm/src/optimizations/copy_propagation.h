// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "optimization.h"

class CopyPropagation : public OptimizationGraph {
 public:
  virtual void optimizeGraph(GraphPtr graph);

 private:
  void optimizeLine(FlowGraph<TInstruction>::Line line);
};
