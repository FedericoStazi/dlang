// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_multi_instruction.h"

#include <memory>

#include "../u_dlang/u_code_builder.h"

UGetAndCheck::UGetAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
                           URegister::Ptr reg, ULocation::Ptr loc)
    : Code<UInstruction>(
      UCodeBuilder(cp, vm)
        .add<UMemCheck>(loc)
        .add<UGet>(reg, loc)
    ) {}

UGetAndCheck::UGetAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
                           URegister::Ptr reg, ULocation::Ptr loc, Tag tag)
    : Code<UInstruction>(
      UCodeBuilder(cp, vm)
      .add<UMemCheck>(loc)
      .add<UTagCheck>(loc->withType(VirtualMachine::Tag), tag)
      .add<UGet>(reg, loc)
    ) {}

USetAndCheck::USetAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
                           ULocation::Ptr loc, UOperand::Ptr oper)
    : Code<UInstruction>(
      UCodeBuilder(cp, vm)
        .add<UMemCheck>(loc)
        .add<USet>(loc, oper)
    ) {}

UMoveAndCheck::UMoveAndCheck(size_t cp, std::shared_ptr<VirtualMachine> vm,
                             URegister::Ptr reg, ULocation::Ptr locFrom,
                             ULocation::Ptr locTo)
    : Code<UInstruction>(
      UCodeBuilder(cp, vm)
        .add<UGetAndCheck>(reg, locFrom)
        .add<USetAndCheck>(locTo, reg)
    ) {}

UMakeHeapPointer::UMakeHeapPointer(size_t cp,
                                   std::shared_ptr<VirtualMachine> vm,
                                   ULocation::Ptr loc, UImmediate::Ptr heapTag)
    : Code<UInstruction>(
      UCodeBuilder(cp, vm)
        .add<USetAndCheck>(loc->withType(VMUArg::Tag), heapTag)
        .add<USetAndCheck>(loc->withType(VMUArg::Val), VMUArg::hp)
    ) {}
