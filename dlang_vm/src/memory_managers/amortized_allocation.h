// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

#include "memory_manager.h"

class AmortizedAllocation : public MemoryManager {
 public:
  virtual void allocateMemory(Memory* memory);
  virtual void collectGarbage(std::shared_ptr<VirtualMachine> vm);
};
