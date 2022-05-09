// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <unordered_set>

#include "../data_structures/code.h"
#include "../data_structures/flow_graph.h"
#include "../t_dlang/t_instruction.h"

class Optimization {
 public:
  virtual Code<TInstruction> optimize(const Code<TInstruction>&) = 0;
};

class OptimizationGraph : public Optimization {
 public:
  virtual Code<TInstruction> optimize(const Code<TInstruction>&);

 protected:
  using GraphPtr = std::shared_ptr<FlowGraph<TInstruction>>;
  using TNodePtr = FlowGraph<TInstruction>::NodePtr;

  virtual void optimizeGraph(GraphPtr graph) = 0;
  void removeNode(const TNodePtr& node);

 private:
  std::unordered_set<TNodePtr> toBeRemoved;
};
