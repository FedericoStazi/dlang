// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "constant_folding.h"

void ConstantFolding::optimizeGraph(GraphPtr graph) {
  for (auto block : graph->getBlocks()) {
    for (auto node : block) {
      // Perform constant folding and remove if not needed
      node->setValue(node->getValue()->fold());
      if (!node->getValue()) {
        removeNode(node);
        continue;
      }
    }
  }
}
