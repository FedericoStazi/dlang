// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "amortized_allocation.h"

#include <algorithm>
#include <memory>

#include "../virtual_machine/memory.h"

void AmortizedAllocation::allocateMemory(Memory* memory) {
  Memory newMemory(std::max(static_cast<size_t>(4), 2 * memory->size()));
  newMemory.copyFrom(*memory);
  *memory = newMemory;
}

void AmortizedAllocation::collectGarbage(std::shared_ptr<VirtualMachine> vm) {}
