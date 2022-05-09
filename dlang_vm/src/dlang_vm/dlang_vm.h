// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <vector>

#include "execution_statistics.h"
#include "timer.h"
#include "../memory_managers/memory_manager.h"
#include "../b_dlang/b_instruction.h"
#include "../jit/jit_state.h"
#include "../jit_policies/jit_policy.h"
#include "../optimizations/optimizations_sequence.h"
#include "../virtual_machine/virtual_machine.h"

enum LogLevel {
  Quiet,       // No output is produced
  Output,      // Program output to stdout (final result or errors)
  Time,        // Timing statistics to stderr
  Statistics,  // All statistics to stderr
  Debug        // Program output to stdout, statistics and debug to stderr
};

template<LogLevel logLevel>
class DlangVM {
 public:
  DlangVM(const Code<BInstruction>& code,
          std::shared_ptr<JITPolicy> jitPolicy,
          std::shared_ptr<MemoryManager> memoryManager,
          std::shared_ptr<OptimizationsSequence> optimizationsSequence);

  int run();

 private:
  void vmLoop();

  // Code of the DLANG program
  const Code<BInstruction>& code_;

  // Virtual Machine used by the program
  std::shared_ptr<VirtualMachine> vm_ = std::make_shared<VirtualMachine>();

  // Structure holding JIT compiled instructions
  std::vector<std::shared_ptr<CompiledInstructions>> compiled_;

  std::shared_ptr<JITPolicy> jitPolicy_;
  std::shared_ptr<MemoryManager> memoryManager_;
  std::shared_ptr<OptimizationsSequence> optimizationsSequence_;

  // Objects storing execution information
  ExecutionStatistics statistics_;
  Timer timer_;
};

#include "dlang_vm.tpp"
