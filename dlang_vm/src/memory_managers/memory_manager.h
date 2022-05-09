// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

class Memory;
class VirtualMachine;

class MemoryManager {
 public:
  virtual void allocateMemory(Memory* memory) = 0;
  virtual void collectGarbage(std::shared_ptr<VirtualMachine> vm) = 0;
};
