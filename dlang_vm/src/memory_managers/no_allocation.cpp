// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "no_allocation.h"

#include <memory>

#include "../virtual_machine/memory.h"
#include "../virtual_machine/exception.h"

NoAllocation::NoAllocation(size_t initialSize) : initialSize_(initialSize) {}

void NoAllocation::allocateMemory(Memory* memory) {
  if (memory->size() == 0) {
    *memory = Memory(initialSize_);
  } else {
    throw RuntimeError();
  }
}

void NoAllocation::collectGarbage(std::shared_ptr<VirtualMachine> vm) {}
