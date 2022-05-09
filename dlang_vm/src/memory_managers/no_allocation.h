// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

#include "memory_manager.h"

class NoAllocation : public MemoryManager {
 public:
  explicit NoAllocation(size_t initialSize = 1'000);

  virtual void allocateMemory(Memory* memory);
  virtual void collectGarbage(std::shared_ptr<VirtualMachine> vm);

 private:
  const size_t initialSize_;
};
