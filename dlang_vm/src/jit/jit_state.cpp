// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "jit_state.h"
#include "jit_vm.h"

#include <algorithm>

JITState::JITState(const JITSequence& jitSequence,
                   std::shared_ptr<VirtualMachine> vm)
    : cps_(jitSequence.getCps()) {
  // Initialize jit compilation the first time
  if (!jitInitialized_) {
    jitInitialized_ = true;
    init_jit(nullptr);
  }

  // Create a new jit_state_t object
  _jit = jit_new_state();

  // Define group of code main entry point
  jit_prolog();

  // Save callee-saved registers on the stack (will be restored automatically)
  jit_frame(0);

  // Load vm state into registers
  jit_ldi(JITVM::sp, &vm->sp);
  jit_ldi(JITVM::fp, &vm->fp);
  jit_ldi(JITVM::cp, &vm->cp);
  jit_ldi(JITVM::hp, &vm->hp);

  for (const auto& [cp, _] : jitSequence.getEntryPoints()) {
    addDirectBranch(jit_beqi(JITVM::cp, cp), cp);
  }
}

void JITState::addLabel(size_t cp, jit_node_t* label) {
  labels_.insert({cp, label});
}

void JITState::addDirectBranch(jit_node_t* label, size_t cp) {
  if (std::find(cps_.begin(), cps_.end(), cp) != cps_.end()) {
    internalBranches_.push_back({label, cp});
  } else {
    externalBranches_.push_back(label);
  }
}

void JITState::addIndirectBranch(jit_node_t* label) {
  externalBranches_.push_back(label);
}

void JITState::addRuntimeErrorBranch(jit_node_t* label) {
  runtimeErrorBranches_.push_back(label);
}

CompiledInstructions JITState::compile(std::shared_ptr<VirtualMachine> vm) {
  // Skip error handling if no error has occurred
  auto noErrorJump = jit_jmpi();

  // Patch all branches caused by runtime errors
  for (const auto& source : runtimeErrorBranches_) {
    jit_patch(source);
  }

  // Set the runtime error flag of the vm
  jit_movi(JITVM::tmp, VirtualMachine::Status::RuntimeError);
  jit_sti(&vm->status, JITVM::tmp);

  // End of error handling
  jit_patch(noErrorJump);

  // Create a label at the end of the group of code
  auto endLabel = jit_label();

  // Patch all branches whose destination is inside the group of code
  for (const auto& [source, destCp] : internalBranches_) {
    jit_patch_at(source, labels_.at(destCp));
  }

  // Patch all branches whose destination is outside the group of code
  for (const auto& source : externalBranches_) {
    jit_patch_at(source, endLabel);
  }

  // Restore vm state from registers
  jit_sti(&vm->sp, JITVM::sp);
  jit_sti(&vm->fp, JITVM::fp);
  jit_sti(&vm->cp, JITVM::cp);
  jit_sti(&vm->hp, JITVM::hp);

  // Compile group of code and save its address
  jit_epilog();
  auto compiled = CompiledInstructions(jit_emit());
  jit_clear_state();
  return compiled;
}
