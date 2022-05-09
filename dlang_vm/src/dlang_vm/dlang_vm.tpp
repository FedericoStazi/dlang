// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "dlang_vm.h"

#include <iostream>

#include "../t_dlang/t_instruction.h"
#include "../t_dlang/out/t_instruction_out.h"
#include "../u_dlang/u_instruction.h"
#include "../u_dlang/out/u_instruction_out.h"
#include "out/execution_statistics_out.h"
#include "out/timer_out.h"
#include "../virtual_machine/out/virtual_machine_out.h"

template<LogLevel logLevel>
DlangVM<logLevel>::DlangVM(const Code<BInstruction>& code,
                 std::shared_ptr<JITPolicy> jitPolicy,
                 std::shared_ptr<MemoryManager> memoryManager,
                 std::shared_ptr<OptimizationsSequence> optimizationsSequence)
    : code_(code),
      compiled_(code_.size()),
      statistics_(code_.size()),
      jitPolicy_(jitPolicy),
      memoryManager_(memoryManager),
      optimizationsSequence_(optimizationsSequence) {}

template<LogLevel logLevel>
int DlangVM<logLevel>::run() {
  // Create the Virtual Machine and allocate memory
  vm_->stack.setMemoryManager(memoryManager_);
  vm_->heap.setMemoryManager(memoryManager_);
  vm_->stack.set(0, {Tag::FramePointer, 0});
  vm_->stack.set(1, {Tag::ReturnAddress, 0});
  vm_->sp = 2;

  // Get start time for timing statistics
  if constexpr (logLevel >= Time) {
    timer_.start();
  }

  // Run the Virtual Machine
  vmLoop();

  // Print timing statistics
  if constexpr (logLevel >= Time) {
    timer_.stop();
    std::cerr << Out::print(timer_);
  }

  // Print final statistics for the computation
  if constexpr (logLevel >= Statistics) {
    std::cerr << Out::print(statistics_);
  }

  // If a runtime error occurred, print error and exit
  if (vm_->status == VirtualMachine::Status::RuntimeError) {
    if constexpr (logLevel != Quiet) {
      std::cout << "output> Runtime error at cp = " << vm_->cp << std::endl;
    }
    return EXIT_FAILURE;
  }

  // Print final result of the computation
  if constexpr (logLevel == Output || logLevel == Debug) {
    std::cout << "output> " << vm_->getResult() << std::endl;
  }

  return EXIT_SUCCESS;
}

template<LogLevel logLevel>
void DlangVM<logLevel>::vmLoop() {
  while (vm_->status == VirtualMachine::Running) {
    if constexpr (logLevel >= Debug) {
      std::cerr << Out::print(vm_);
    }

    // Run Garbage Collection
    memoryManager_->collectGarbage(vm_);

    // Count landings for this instruction
    jitPolicy_->notifyLanding(vm_->cp);

    if (!compiled_[vm_->cp]) {
      // Get the jit compilation policy
      auto jitSequence = jitPolicy_->makeJITSequence(code_, vm_->cp);

      if (!jitSequence.isEmpty()) {
        // Create u-code instructions
        Code<UInstruction> uCode;
        for (auto cp : jitSequence.getCps()) {
          uCode += code_.getInstruction(cp)->getUInstructions(vm_);
        }

        // Optimize u-code
        Code<UInstruction> uCodeOptimized;
        if (jitSequence.isFunction()) {
          uCodeOptimized = optimizationsSequence_->optimizeFunction(uCode);
        } else {
          uCodeOptimized = optimizationsSequence_->optimizeTrace(uCode);
        }

        // Compile u-code while keeping group-level jit state
        auto jit = std::make_shared<JITState>(jitSequence, vm_);
        for (auto uInstruction : uCodeOptimized) {
          uInstruction->jitCompile(vm_, jit);
        }

        // Store the pointer for the main and all secondary entry points
        compiled_[vm_->cp] =
            std::make_shared<CompiledInstructions>(jit->compile(vm_));
        statistics_.addCompiled(vm_->cp, jitSequence.getCps().size());
        for (const auto& [cp, size] : jitSequence.getEntryPoints()) {
          compiled_[cp] = compiled_[vm_->cp];
          statistics_.addCompiled(cp, size);
        }

        // Print compilation statistics
        if constexpr (logLevel >= Statistics) {
          std::cout << "Compiled instructions: ";
          for (auto cp : jitSequence.getCps()) {
            std::cout << cp << " ";
          }
          std::cout << std::endl;
          std::cout << Out::print(uCode) << std::endl;
          std::cout << Out::print(uCodeOptimized) << std::endl;
          std::cout << "Original code size: " << uCode.size() << std::endl;
          std::cout << "Optimized code size: " << uCodeOptimized.size()
                    << std::endl << std::endl;
        }
      }
    }

    // Interpret the instruction or run its compiled code
    if (compiled_[vm_->cp]) {
      statistics_.countRunJIT(vm_->cp);
      jitPolicy_->notifyRunJIT(vm_->cp);
      compiled_[vm_->cp]->run();
    } else {
      statistics_.countInterpreted(vm_->cp);
      try {
        code_.getInstruction(vm_->cp)->interpret(vm_);
      } catch (const RuntimeError&) {
        vm_->status = VirtualMachine::Status::RuntimeError;
      }
    }
  }
}
