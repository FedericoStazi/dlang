// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>

#include "../jit/jit.h"
#include "../jit/jit_vm.h"
#include "../t_dlang/t_argument.h"
#include "../virtual_machine/memory.h"
#include "../virtual_machine/virtual_machine.h"

class TArgument;
class TImmediate;
class TVariable;

class UArgument {
 public:
  using Ptr = std::shared_ptr<UArgument>;
  virtual TArgument::Ptr
      makeTArgument(std::shared_ptr<TState> tArgumentsState) = 0;
  virtual std::string print() const = 0;

 protected:
  using VMPtr = std::shared_ptr<VirtualMachine>;
};

class UOperand : public UArgument {
 public:
  using Ptr = std::shared_ptr<UOperand>;
};

class UImmediate : public UOperand,
                   public std::enable_shared_from_this<UImmediate> {
 public:
  using Ptr = std::shared_ptr<UImmediate>;
  virtual int getValue() const = 0;
  TArgument::Ptr makeTArgument(std::shared_ptr<TState> tArgumentsState);
};

class URegister : public UOperand,
                  public std::enable_shared_from_this<URegister> {
 public:
  using Ptr = std::shared_ptr<URegister>;
  URegister(std::string name, jit_reg_t reg);

  jit_reg_t getReg() const;
  std::string print() const;
  TArgument::Ptr makeTArgument(std::shared_ptr<TState> tArgumentsState);

 private:
  jit_reg_t reg_;
  std::string name_;
};

class ULocation : public UArgument {
 public:
  using Ptr = std::shared_ptr<ULocation>;
  ULocation(URegister::Ptr ptr, int offset, VirtualMachine::Type type);

  URegister::Ptr getPtr() const;
  int getOffset() const;
  VirtualMachine::Type getType() const;

  virtual ULocation::Ptr withType(VirtualMachine::Type type) = 0;
  virtual Memory* getMemoryPtr(VMPtr vm) const = 0;
  virtual Item** getDataPtr(VMPtr vm) const = 0;
  virtual size_t* getSizePtr(VMPtr vm) const = 0;

  std::string print() const;

 private:
  URegister::Ptr ptr_;
  int offset_;
  VirtualMachine::Type type_;
};

class UImmInt : public UImmediate {
 public:
  using Ptr = std::shared_ptr<UImmInt>;
  explicit UImmInt(int value);

  virtual int getValue() const;
  std::string print() const;

 private:
  int value_;
};

class UImmTag : public UImmediate {
 public:
  using Ptr = std::shared_ptr<UImmTag>;
  explicit UImmTag(Tag value);

  virtual int getValue() const;
  std::string print() const;

 private:
  Tag value_;
};

class UImmStatus : public UImmediate {
 public:
  using Ptr = std::shared_ptr<UImmStatus>;
  explicit UImmStatus(VirtualMachine::Status value);

  virtual int getValue() const;
  std::string print() const;

 private:
  VirtualMachine::Status value_;
};

class URegGP : public URegister {
 public:
  using Ptr = std::shared_ptr<URegGP>;
  URegGP(std::string name, jit_reg_t reg);
};

class URegSP : public URegister {
 public:
  using Ptr = std::shared_ptr<URegSP>;
  URegSP(std::string name, jit_reg_t reg);
};

class ULocStack : public ULocation {
 public:
  using Ptr = std::shared_ptr<ULocStack>;
  ULocStack(URegister::Ptr ptr, int offset, VirtualMachine::Type type);

  virtual Memory* getMemoryPtr(VMPtr vm) const;
  virtual Item** getDataPtr(VMPtr vm) const;
  virtual size_t* getSizePtr(VMPtr vm) const;
};

class ULocHeap : public ULocation,
                 public std::enable_shared_from_this<ULocHeap> {
 public:
  using Ptr = std::shared_ptr<ULocHeap>;
  ULocHeap(URegister::Ptr ptr, size_t offset, VirtualMachine::Type type);

  virtual Memory* getMemoryPtr(VMPtr vm) const;
  virtual Item** getDataPtr(VMPtr vm) const;
  virtual size_t* getSizePtr(VMPtr vm) const;
  virtual ULocation::Ptr withType(VirtualMachine::Type type);
  TArgument::Ptr makeTArgument(std::shared_ptr<TState> tArgumentsState);
};

class ULocSP : public ULocStack, public std::enable_shared_from_this<ULocSP> {
 public:
  using Ptr = std::shared_ptr<ULocSP>;
  ULocSP(size_t offset, VirtualMachine::Type type);

  virtual ULocation::Ptr withType(VirtualMachine::Type type);
  TArgument::Ptr makeTArgument(std::shared_ptr<TState> tArgumentsState);
};

class ULocFP : public ULocStack, public std::enable_shared_from_this<ULocFP> {
 public:
  using Ptr = std::shared_ptr<ULocFP>;
  ULocFP(size_t offset, VirtualMachine::Type type);

  virtual ULocation::Ptr withType(VirtualMachine::Type type);
  TArgument::Ptr makeTArgument(std::shared_ptr<TState> tArgumentsState);
};

class VMUArg : public JIT {
 public:
  static UImmediate::Ptr uImm(int value);
  static UImmediate::Ptr uImm(Tag value);
  static UImmediate::Ptr uImm(VirtualMachine::Status value);

  static inline auto sp {std::make_shared<URegSP>("sp", JITVM::sp)};
  static inline auto fp {std::make_shared<URegSP>("fp", JITVM::fp)};
  static inline auto cp {std::make_shared<URegSP>("cp", JITVM::cp)};
  static inline auto hp {std::make_shared<URegSP>("hp", JITVM::hp)};

  static inline auto r0 {std::make_shared<URegGP>("r0", JIT_R0)};
  static inline auto r1 {std::make_shared<URegGP>("r1", JIT_R1)};
  static inline auto r2 {std::make_shared<URegGP>("r2", JIT_R2)};

  static const auto Val = VirtualMachine::Type::Val;
  static const auto Tag = VirtualMachine::Type::Tag;

  static ULocHeap::Ptr Heap(URegister::Ptr reg, int offset,
                            VirtualMachine::Type type);
  static ULocFP::Ptr FP(int offset, VirtualMachine::Type type);
  static ULocSP::Ptr SP(int offset, VirtualMachine::Type type);
  static ULocHeap::Ptr HP(int offset, VirtualMachine::Type type);
};
