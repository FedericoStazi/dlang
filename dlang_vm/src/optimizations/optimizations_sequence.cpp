// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "optimizations_sequence.h"

#include "../u_dlang/u_instruction.h"
#include "../virtual_machine/exception.h"

void OptimizationsSequence::add(std::shared_ptr<Optimization> optimization) {
  optimizations_.push_back(optimization);
}

Code<UInstruction>
    OptimizationsSequence::optimizeFunction(const Code<UInstruction>& uCode) {
  auto tState = std::make_shared<TState>();
  tState->setFunction();
  try {
    return optimize(uCode, tState);
  } catch (const OptimizationError&) {
    return uCode;
  }
}

Code<UInstruction>
    OptimizationsSequence::optimizeTrace(const Code<UInstruction>& uCode) {
  auto tState = std::make_shared<TState>();
  try {
    return optimize(uCode, tState);
  } catch (const OptimizationError&) {
    return uCode;
  }
}

Code<UInstruction>
    OptimizationsSequence::optimize(const Code<UInstruction>& uCode,
                                    std::shared_ptr<TState> tState) {
  // Create t-code instructions
  Code<TInstruction> tCode;
  for (auto instruction : uCode) {
    tCode.add(instruction->getTInstruction(tState));
  }

  // Optimize t-code
  for (const auto& optimization : optimizations_) {
    tCode = optimization->optimize(tCode);
  }

  // Create optimized u-code instructions
  Code<UInstruction> uCodeOptimized;
  for (auto instruction : tCode) {
    uCodeOptimized.add(instruction->getUInstruction());
  }

  return uCodeOptimized;
}
