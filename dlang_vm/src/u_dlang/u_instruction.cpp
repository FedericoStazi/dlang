// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_instruction.h"

UInstruction::UInstruction(size_t cp, VMPtr vm) : cp(cp), vm(vm) {}

UGet::UGet(size_t cp, VMPtr vm, URegister::Ptr a, ULocation::Ptr b)
    : UInstruction(cp, vm), a(a), b(b) {}

USet::USet(size_t cp, VMPtr vm, ULocation::Ptr a, UOperand::Ptr b)
    : UInstruction(cp, vm), a(a), b(b) {}

UMove::UMove(size_t cp, VMPtr vm, URegister::Ptr a,
             UOperand::Ptr b)
    : UInstruction(cp, vm), a(a), b(b) {}

UUnary::UUnary(size_t cp, VMPtr vm, UnaryOp op, URegister::Ptr a,
               UOperand::Ptr b)
    : UInstruction(cp, vm), op(op), a(a), b(b) {}

UOper::UOper(size_t cp, VMPtr vm, BinaryOp op, URegister::Ptr a,
             UOperand::Ptr b, UOperand::Ptr c)
    : UInstruction(cp, vm), op(op), a(a), b(b), c(c) {}

ULabel::ULabel(size_t cp, VMPtr vm) : UInstruction(cp, vm) {}

UGuard::UGuard(size_t cp, VMPtr vm) : UInstruction(cp, vm) {}

UMemCheck::UMemCheck(size_t cp, VMPtr vm, ULocation::Ptr a)
    : UInstruction(cp, vm), a(a) {}

UTagCheck::UTagCheck(size_t cp, VMPtr vm, UArgument::Ptr a, Tag tagA)
    : UInstruction(cp, vm), a(a), tagA(tagA), tagB(tagA) {}

UTagCheck::UTagCheck(size_t cp, VMPtr vm, UArgument::Ptr a, Tag tagA, Tag tagB)
    : UInstruction(cp, vm), a(a), tagA(tagA), tagB(tagB) {}

UApply::UApply(size_t cp, VMPtr vm) : UInstruction(cp, vm) {}

UReturn::UReturn(size_t cp, VMPtr vm) : UInstruction(cp, vm) {}

UHalt::UHalt(size_t cp, VMPtr vm) : UInstruction(cp, vm) {}

UGoto::UGoto(size_t cp, VMPtr vm, size_t destination)
    : UInstruction(cp, vm), destination(destination) {}

UBranch::UBranch(size_t cp, VMPtr vm, URegister::Ptr a, size_t destination)
    : UInstruction(cp, vm), a(a), destination(destination) {}
