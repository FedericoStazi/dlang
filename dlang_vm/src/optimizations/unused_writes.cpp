// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "unused_writes.h"

#include "../u_dlang/u_instruction.h"

void RemoveUnusedWrites::optimizeGraph(GraphPtr graph) {
  for (auto block : graph->getBlocks()) {
    std::unordered_map<TArgument::Ptr, TNodePtr> unusedWrite;
    for (auto node : block) {
      for (const auto& var : node->getValue()->getWriteArgs()) {
        if (std::dynamic_pointer_cast<ULocSP>(var->getUArgument()) ||
            std::dynamic_pointer_cast<ULocFP>(var->getUArgument()) ||
            std::dynamic_pointer_cast<URegGP>(var->getUArgument())) {
          if (unusedWrite.count(var)) {
            removeNode(unusedWrite.at(var));
          }
          unusedWrite[var] = node;
        }
      }
    }
  }
}
