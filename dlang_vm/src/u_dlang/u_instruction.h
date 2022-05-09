// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>

#include "u_argument.h"
#include "../jit/jit_vm.h"
#include "../jit/jit_state.h"
#include "../virtual_machine/exception.h"
#include "../virtual_machine/operations.h"
#include "../virtual_machine/runtime_system.h"
#include "../virtual_machine/virtual_machine.h"

class TInstruction;

// Virtual class representing an individual micro-instruction
class UInstruction : public JIT,
                     public std::enable_shared_from_this<UInstruction> {
 protected:
  using VMPtr = std::shared_ptr<VirtualMachine>;
  using JITPtr = std::shared_ptr<JITState>;

 public:
  explicit UInstruction(size_t cp, VMPtr vm);

  // Emits jit code according to the instruction definition
  virtual void jitCompile(VMPtr vm, JITPtr jit) const = 0;

  // Generate a TInstruction object
  virtual std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState) = 0;

  virtual std::string print() const = 0;

  const size_t cp;
  VMPtr vm;
};

class UGet : virtual public UInstruction {
 public:
  UGet(size_t cp, VMPtr vm, URegister::Ptr a, ULocation::Ptr b);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  URegister::Ptr a;
  ULocation::Ptr b;
};

class USet : virtual public UInstruction {
 public:
  USet(size_t cp, VMPtr vm, ULocation::Ptr a, UOperand::Ptr b);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  ULocation::Ptr a;
  UOperand::Ptr b;
};

class UMove : virtual public UInstruction {
 public:
  UMove(size_t cp, VMPtr vm, URegister::Ptr a, UOperand::Ptr b);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  URegister::Ptr a;
  UOperand::Ptr b;
};

class UUnary : virtual public UInstruction {
 public:
  UUnary(size_t cp, VMPtr vm,
         UnaryOp op, URegister::Ptr a, UOperand::Ptr b);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  UnaryOp op;
  URegister::Ptr a;
  UOperand::Ptr b;
};

class UOper : virtual public UInstruction {
 public:
  UOper(size_t cp, VMPtr vm,
        BinaryOp op, URegister::Ptr a, UOperand::Ptr b, UOperand::Ptr c);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  BinaryOp op;
  URegister::Ptr a;
  UOperand::Ptr b, c;
};

class ULabel : virtual public UInstruction {
 public:
  ULabel(size_t cp, VMPtr vm);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;
};

class UGuard : virtual public UInstruction {
 public:
  UGuard(size_t cp, VMPtr vm);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;
};

class UMemCheck : virtual public UInstruction {
 public:
  UMemCheck(size_t cp, VMPtr vm, ULocation::Ptr a);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  ULocation::Ptr a;
};

class UTagCheck : virtual public UInstruction {
 public:
  UTagCheck(size_t cp, VMPtr vm, UArgument::Ptr a, Tag tagA);
  UTagCheck(size_t cp, VMPtr vm, UArgument::Ptr a, Tag tagA, Tag tagB);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  UArgument::Ptr a;
  Tag tagA, tagB;
};

class UApply : virtual public UInstruction {
 public:
  UApply(size_t cp, VMPtr vm);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;
};

class UReturn : virtual public UInstruction {
 public:
  UReturn(size_t cp, VMPtr vm);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;
};

class UHalt : virtual public UInstruction {
 public:
  UHalt(size_t cp, VMPtr vm);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;
};

class UGoto : virtual public UInstruction {
 public:
  UGoto(size_t cp, VMPtr vm, size_t destination);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  size_t destination;
};

class UBranch : virtual public UInstruction {
 public:
  UBranch(size_t cp, VMPtr vm, URegister::Ptr a, size_t destination);

  void jitCompile(VMPtr vm, JITPtr jit) const;
  std::shared_ptr<TInstruction>
      getTInstruction(std::shared_ptr<TState> tState);

  std::string print() const;

  URegister::Ptr a;
  size_t destination;
};
