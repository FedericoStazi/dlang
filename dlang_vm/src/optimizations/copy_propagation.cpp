// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "copy_propagation.h"

#include "../u_dlang/u_instruction.h"

void CopyPropagation::optimizeLine(FlowGraph<TInstruction>::Line line) {
  for (auto it = line.begin(); it != line.end(); it++) {
    auto& node = *it;

    // Check if this is a move instruction
    auto move = std::dynamic_pointer_cast<TMove>(node->getValue());
    if (!move || std::dynamic_pointer_cast<UGet>(move->getUInstruction())) {
      continue;
    }

    auto varA = std::dynamic_pointer_cast<TVariable>(move->a);
    if (!varA) { continue; }
    auto varB = std::dynamic_pointer_cast<TVariable>(move->b);

    // Iterate over the next instructions in the block
    for (auto itProp = std::next(it); itProp != line.end(); itProp++) {
      auto& nodeProp = *itProp;

      // Propagate the copy
      nodeProp->getValue()->propagateCopy(varA, move->b);

      // Stop if a or b is modified
      bool breakLoop = false;
      for (auto arg : nodeProp->getValue()->getWriteArgs()) {
        auto var = std::dynamic_pointer_cast<TVariable>(arg);
        if (*var == *varA) { breakLoop = true; }
        if (varB && *var == *varB) { breakLoop = true; }
      }
      if (breakLoop) { break; }
    }
  }
}

void CopyPropagation::optimizeGraph(GraphPtr graph) {
  if (graph->getStartNode()->getValue()->isFunction()) {
    for (auto block : graph->getBlocks()) {
      optimizeLine(block);
    }
  } else {
    optimizeLine(graph->getLine());
  }
}
