// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>

#include "../virtual_machine/memory.h"

// Object holding the machine state
class VirtualMachine {
 public:
  enum Location { Stack, Heap };
  enum Type { Tag, Val };

  // Virtual Machine state
  enum Status {Halted, Running, RuntimeError} status = Running;
  size_t sp = 0, fp = 0, cp = 0, hp = 0;
  Memory stack, heap;

  // Get the result from the vm (a string representing the top of the stack)
  std::string getResult();

 private:
  // Get the string representation of item
  std::string toString(Item item);
};
