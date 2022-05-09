// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

#include "../virtual_machine/operations.h"
#include "../virtual_machine/runtime_system.h"
#include "../virtual_machine/virtual_machine.h"
#include "u_instruction.h"
#include "../data_structures/code.h"

class UGetAndCheck : public Code<UInstruction> {
 public:
  UGetAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
               URegister::Ptr reg, ULocation::Ptr loc);
  UGetAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
               URegister::Ptr reg, ULocation::Ptr loc, Tag tag);
};

class USetAndCheck : public Code<UInstruction> {
 public:
  USetAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
               ULocation::Ptr loc, UOperand::Ptr oper);
};

class UMoveAndCheck : public Code<UInstruction> {
 public:
  UMoveAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
                URegister::Ptr reg, ULocation::Ptr locFrom,
                ULocation::Ptr locTo);
};

class UMakeHeapPointer : public Code<UInstruction> {
 public:
  UMakeHeapPointer(size_t cp, std::shared_ptr<VirtualMachine> vm,
                   ULocation::Ptr loc, UImmediate::Ptr heapTag);
};
