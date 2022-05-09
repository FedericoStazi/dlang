// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

#include "b_group.h"
#include "../virtual_machine/operations.h"
#include "../virtual_machine/virtual_machine.h"
#include "../data_structures/code.h"
#include "../virtual_machine/exception.h"

class UInstruction;

// Virtual class representing an individual instruction
class BInstruction : virtual public BGroupRole<Function>,
                     virtual public BGroupRole<Block>,
                     virtual public BGroupRole<Individual>,
                     public StartOf<Individual>,
                     public EndOf<Individual> {
 protected:
  using VMPtr = std::shared_ptr<VirtualMachine>;

 public:
  explicit BInstruction(size_t cp);

  // Modifies the vm state according to the instruction definition
  // (i.e. the instruction is interpreted)
  virtual void interpret(VMPtr vm) const = 0;

  virtual Code<UInstruction> getUInstructions(VMPtr vm) const = 0;

  size_t getCp() const;

 private:
  const size_t cp_;
};

class BUnary : virtual public BInstruction,
               public StartOf<Block> {
 public:
  using Op = UnaryOp;
  explicit BUnary(size_t cp, Op op);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  Op op_;
};

class BOper : virtual public BInstruction,
              public StartOf<Block> {
 public:
  using Op = BinaryOp;
  explicit BOper(size_t cp, Op op);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  Op op_;
};

class BMkPair : virtual public BInstruction,
                public StartOf<Block> {
 public:
  explicit BMkPair(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BFst : virtual public BInstruction,
             public StartOf<Block> {
 public:
  explicit BFst(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BSnd : virtual public BInstruction,
             public StartOf<Block> {
 public:
  explicit BSnd(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BMkInl : virtual public BInstruction,
               public StartOf<Block> {
 public:
  explicit BMkInl(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BMkInr : virtual public BInstruction,
               public StartOf<Block> {
 public:
  explicit BMkInr(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BPush : virtual public BInstruction,
              public StartOf<Block> {
 public:
  enum Tag { Unit, Bool, Int };
  BPush(size_t cp, Tag tag, int value);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  Tag tag_;
  int value_;
};

class BApply : virtual public BInstruction,
               public EndOf<Block>,
               public BeforeEntryOf<Function> {
 public:
  explicit BApply(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BLookup : virtual public BInstruction,
                public StartOf<Block> {
 public:
  using Location = VirtualMachine::Location;
  BLookup(size_t cp, Location location, int offset);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  Location location_;
  int offset_;
};

class BReturn : virtual public BInstruction,
                public EndOf<Block>,
                public EndOf<Function>  {
 public:
  explicit BReturn(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BMkClosure : virtual public BInstruction,
                   public StartOf<Block> {
 public:
  BMkClosure(size_t cp, size_t location, size_t size);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  size_t location_;
  size_t size_;
};

class BSwap : virtual public BInstruction,
              public StartOf<Block> {
 public:
  explicit BSwap(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BPop : virtual public BInstruction,
             public StartOf<Block> {
 public:
  explicit BPop(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BLabel : virtual public BInstruction,
               public StartOf<Block> {
 public:
  explicit BLabel(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BFunction: virtual public BInstruction,
                 public StartOf<Block>,
                 public StartOf<Function>,
                 public BeforeEntryOf<Function> {
 public:
  explicit BFunction(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BDeref : virtual public BInstruction,
               public StartOf<Block> {
 public:
  explicit BDeref(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BMkRef : virtual public BInstruction,
               public StartOf<Block> {
 public:
  explicit BMkRef(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BAssign : virtual public BInstruction,
                public StartOf<Block> {
 public:
  explicit BAssign(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BHalt : virtual public BInstruction,
              public EndOf<Block>,
              public EndOf<Function> {
 public:
  explicit BHalt(size_t cp);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
};

class BGoto : virtual public BInstruction,
              public EndOf<Block> {
 public:
  BGoto(size_t cp, size_t destination);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  size_t destination_;
};

class BTest : virtual public BInstruction,
              public EndOf<Block> {
 public:
  BTest(size_t cp, size_t destination);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  size_t destination_;
};

class BCase : virtual public BInstruction,
              public EndOf<Block> {
 public:
  BCase(size_t cp, size_t destination);
  void interpret(VMPtr vm) const;
  Code<UInstruction> getUInstructions(VMPtr vm) const;
 private:
  size_t destination_;
};
