// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include <fstream>
#include <iostream>

#include "b_dlang/b_code_builder.h"
#include "options/options.h"
#include "dlang_vm/dlang_vm.h"
#include "memory_managers/amortized_allocation.h"
#include "memory_managers/no_allocation.h"
#include "memory_managers/mark_and_sweep_gc.h"
#include "jit_policies/no_jit.h"
#include "jit_policies/group_jit.h"
#include "jit_policies/tracing_jit.h"
#include "optimizations/constant_folding.h"
#include "optimizations/copy_propagation.h"
#include "optimizations/dead_code.h"
#include "optimizations/optimizations_sequence.h"
#include "optimizations/redundant_checks.h"
#include "optimizations/unused_writes.h"

int main(int argc, char** argv) {
  // Parse command line arguments
  Options options(argc, argv);

  // Print help message and stop if arguments include "--help"
  if (options.count("help")) {
    std::cout << options.getDescription();
    return EXIT_SUCCESS;
  }

  // Print help message and stop if arguments do not include the file
  if (!options.count("file")) {
    std::cout << options.getDescription();
    return EXIT_FAILURE;
  }

  // Get the options from the command line
  auto verbosityOption = options["verbosity"].as<std::string>();
  auto threshold = options["jit-threshold"].as<size_t>();
  auto jitPolicyOption = options["jit-policy"].as<std::string>();
  auto memoryOption = options["memory"].as<std::string>();
  auto optimizationsOption = options["optimizations"].as<std::string>();

  std::shared_ptr<JITPolicy> jitPolicy;
  std::shared_ptr<MemoryManager> memoryManager;
  auto optimizationsSequence = std::make_shared<OptimizationsSequence>();

  if (jitPolicyOption == "no") {
    jitPolicy = std::make_shared<NoJIT>();
  } else if (jitPolicyOption == "tracing") {
    jitPolicy = std::make_shared<TracingJIT>(threshold);
  } else if (jitPolicyOption == "individual") {
    jitPolicy = std::make_shared<GroupJIT<Individual>>(threshold);
  } else if (jitPolicyOption == "block") {
    jitPolicy = std::make_shared<GroupJIT<Block>>(threshold);
  } else if (jitPolicyOption == "function") {
    jitPolicy = std::make_shared<GroupJIT<Function>>(threshold);
  } else {
    std::cout << "Sequence " << jitPolicyOption << " is not valid" << std::endl;
    return EXIT_FAILURE;
  }

  if (memoryOption == "none") {
    memoryManager = std::make_shared<NoAllocation>();
  } else if (memoryOption == "amortized") {
    memoryManager = std::make_shared<AmortizedAllocation>();
  } else if (memoryOption == "mark-and-sweep") {
    memoryManager = std::make_shared<MarkAndSweepGC>();
  } else {
    std::cout << "Memory " << memoryOption << " is not valid" << std::endl;
    return EXIT_FAILURE;
  }

  if (!optimizationsOption.empty()) {
    optimizationsOption += ",";
    for (auto it = optimizationsOption.begin();
        it != optimizationsOption.end();
        it = std::find(it, optimizationsOption.end(), ',') + 1) {
      std::string optimization(it,
                               std::find(it, optimizationsOption.end(), ','));
      if (optimization == "redundant-checks") {
        optimizationsSequence->add(std::make_shared<RemoveRedundantChecks>());
      } else if (optimization == "unused-writes") {
        optimizationsSequence->add(std::make_shared<RemoveUnusedWrites>());
      } else if (optimization == "copy-propagation") {
        optimizationsSequence->add(std::make_shared<CopyPropagation>());
      } else if (optimization == "dead-code") {
        optimizationsSequence->add(std::make_shared<DeadCodeElimination>());
      } else if (optimization == "constant-folding") {
        optimizationsSequence->add(std::make_shared<ConstantFolding>());
      } else {
        std::cout << "Optimization " << optimization
                  << " is not valid" << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  // Read contents of the bytecode file
  std::string filename = options["file"].as<std::string>();
  std::ifstream fileStream(filename);
  std::string codeString((std::istreambuf_iterator<char>(fileStream)),
                          std::istreambuf_iterator<char>());
  auto code = BCodeBuilder::fromString(codeString);

  if (verbosityOption == "quiet") {
    DlangVM<Quiet>(code, jitPolicy, memoryManager,
                   optimizationsSequence).run();
  } else if (verbosityOption == "output") {
    DlangVM<Output>(code, jitPolicy, memoryManager,
                    optimizationsSequence).run();
  } else if (verbosityOption == "time") {
    DlangVM<Time>(code, jitPolicy, memoryManager,
                  optimizationsSequence).run();
  } else if (verbosityOption == "statistics") {
    DlangVM<Statistics>(code, jitPolicy, memoryManager,
                        optimizationsSequence).run();
  } else if (verbosityOption == "debug") {
    DlangVM<Debug>(code, jitPolicy, memoryManager,
                   optimizationsSequence).run();
  } else {
    std::cout << "Verbosity " << verbosityOption
              << " is not valid" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
