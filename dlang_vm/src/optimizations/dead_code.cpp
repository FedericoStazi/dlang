// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "dead_code.h"

#include <algorithm>

#include "../data_structures/int_set.h"

void DeadCodeElimination::optimizeGraph(GraphPtr graph) {
  // The set of live variables
  std::unordered_map<TNodePtr, IntSet> live;

  // Reverse the node order for better performance
  auto nodes = graph->getLine();
  std::reverse(nodes.begin(), nodes.end());

  // Create the live sets of the nodes
  bool liveHasChanged;
  do {
    liveHasChanged = false;
    for (const auto& node : nodes) {
      auto liveOld = live[node];
      live[node] = {};
      for (auto succ : node->getSuccessors()) {
        live[node].insert(live[succ]);
      }
      for (const auto& arg : node->getValue()->getWriteArgs()) {
        if (auto var = std::dynamic_pointer_cast<TVariable>(arg)) {
          live[node].erase(var->getUID());
        }
      }
      for (const auto& arg : node->getValue()->getReadArgs()) {
        if (auto var = std::dynamic_pointer_cast<TVariable>(arg)) {
          live[node].insert(var->getUID());
        }
      }
      liveHasChanged |= liveOld != live[node];
    }
  } while (liveHasChanged);

  // Delete nodes whose effects are not needed
  for (auto node : graph->getLine()) {
    if (auto var = node->getValue()->getEffects().getWrite()) {
      for (auto succ : node->getSuccessors()) {
        if (live[succ].get(var->getUID())) { break; }
        removeNode(node);
      }
    }
  }
}
