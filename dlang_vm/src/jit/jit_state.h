// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "compiled_instructions.h"
#include "jit.h"
#include "../jit_policies/jit_sequence.h"
#include "../virtual_machine/virtual_machine.h"

class JITState : public JIT {
 public:
  JITState(const JITSequence& jitSequence, std::shared_ptr<VirtualMachine> vm);

  // Labels are used as branch destinations
  void addLabel(size_t cp, jit_node_t* label);

  // Entry points are the possible values of JITVM::cp when the code is called
  std::vector<size_t> getEntryPoints() const;

  // Add a branch with fixed destination
  void addDirectBranch(jit_node_t* label, size_t cp);

  // Add a branch with non-fixed destination
  void addIndirectBranch(jit_node_t* label);

  // Add a branch to the runtime error handling code
  void addRuntimeErrorBranch(jit_node_t* label);

  // JIT Compile the emitted instructions
  CompiledInstructions compile(std::shared_ptr<VirtualMachine> vm);

 private:
  // Code pointers for this instruction
  const std::vector<size_t> cps_;

  // Labels for this code section
  std::unordered_map<size_t, jit_node_t*> labels_;

  // Branches into the code, outside of the code or to the error handler
  std::vector<std::pair<jit_node_t*, size_t>> internalBranches_;
  std::vector<jit_node_t*> externalBranches_;
  std::vector<jit_node_t*> runtimeErrorBranches_;

  // True if init_jit was called, false otherwise
  inline static bool jitInitialized_ = false;
};
