// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "optimization.h"

#include <iostream>
#include <memory>

#include "../data_structures/code.h"

Code<TInstruction> OptimizationGraph::optimize(const Code<TInstruction>& code) {
  // Construct the flow graph
  auto graph = std::make_shared<FlowGraph<TInstruction>>();
  for (const auto& tInstruction : code) {
    tInstruction->makeFlowGraph(graph);
  }

  // Optimize the graph by removing nodes
  optimizeGraph(graph);

  // Re-construct code from non-removed nodes
  Code<TInstruction> optimizedCode;
  for (auto node : graph->getLine()) {
    if (!toBeRemoved.count(node)) {
      optimizedCode.add(node->getValue());
    }
  }
  return optimizedCode;
}

void OptimizationGraph::removeNode(const TNodePtr& node) {
  toBeRemoved.insert(node);
}
