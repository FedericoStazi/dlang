// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_argument.h"

TArgument::Ptr
    UImmediate::makeTArgument(std::shared_ptr<TState> tArgumentsState) {
  return tArgumentsState->makeTArgument(shared_from_this());
}

URegister::URegister(std::string name, jit_reg_t reg)
    : name_(name), reg_(reg) {}

jit_reg_t URegister::getReg() const {
  return reg_;
}

TArgument::Ptr
    URegister::makeTArgument(std::shared_ptr<TState> tArgumentsState) {
  return tArgumentsState->makeTArgument(shared_from_this());
}

ULocation::ULocation(URegister::Ptr ptr, int offset, VirtualMachine::Type type)
    : ptr_(ptr), offset_(offset), type_(type) {}

URegister::Ptr ULocation::getPtr() const {
  return ptr_;
}

int ULocation::getOffset() const {
  return offset_;
}

VirtualMachine::Type ULocation::getType() const {
  return type_;
}

UImmInt::UImmInt(int value) : value_(value) {}

int UImmInt::getValue() const {
  return value_;
}

UImmTag::UImmTag(Tag value) : value_(value) {}

int UImmTag::getValue() const {
  return value_;
}

UImmStatus::UImmStatus(VirtualMachine::Status value) : value_(value) {}

int UImmStatus::getValue() const {
  return value_;
}

URegGP::URegGP(std::string name, jit_reg_t reg) : URegister(name, reg) {}

URegSP::URegSP(std::string name, jit_reg_t reg) : URegister(name, reg) {}

ULocStack::ULocStack(URegister::Ptr ptr, int offset, VirtualMachine::Type type)
    : ULocation(ptr, offset, type) {}

Memory* ULocStack::getMemoryPtr(VMPtr vm) const {
  return &vm->stack;
}

Item** ULocStack::getDataPtr(VMPtr vm) const {
  return vm->stack.getDataPtr();
}

size_t* ULocStack::getSizePtr(VMPtr vm) const {
  return vm->stack.getSizePtr();
}

ULocHeap::ULocHeap(URegister::Ptr ptr, size_t offset, VirtualMachine::Type type)
    : ULocation(ptr, offset, type) {}

Memory* ULocHeap::getMemoryPtr(VMPtr vm) const {
  return &vm->heap;
}

Item** ULocHeap::getDataPtr(VMPtr vm) const {
  return vm->heap.getDataPtr();
}

size_t* ULocHeap::getSizePtr(VMPtr vm) const {
  return vm->heap.getSizePtr();
}

ULocation::Ptr ULocHeap::withType(VirtualMachine::Type type) {
  return std::make_shared<ULocHeap>(getPtr(), getOffset(), type);
}

TArgument::Ptr
    ULocHeap::makeTArgument(std::shared_ptr<TState> tArgumentsState) {
  return tArgumentsState->makeTArgument(shared_from_this());
}

ULocSP::ULocSP(size_t offset, VirtualMachine::Type type)
    : ULocStack(std::make_shared<URegSP>("sp", JITVM::sp), offset, type) {}

ULocation::Ptr ULocSP::withType(VirtualMachine::Type type) {
  return std::make_shared<ULocSP>(getOffset(), type);
}

TArgument::Ptr ULocSP::makeTArgument(std::shared_ptr<TState> tArgumentsState) {
  return tArgumentsState->makeTArgument(shared_from_this());
}

ULocFP::ULocFP(size_t offset, VirtualMachine::Type type)
    : ULocStack(std::make_shared<URegSP>("fp", JITVM::fp), offset, type) {}

ULocation::Ptr ULocFP::withType(VirtualMachine::Type type) {
  return std::make_shared<ULocFP>(getOffset(), type);
}

TArgument::Ptr ULocFP::makeTArgument(std::shared_ptr<TState> tArgumentsState) {
  return tArgumentsState->makeTArgument(shared_from_this());
}

UImmediate::Ptr VMUArg::uImm(int value) {
  return std::make_shared<UImmInt>(value);
}

UImmediate::Ptr VMUArg::uImm(::Tag value) {
  return std::make_shared<UImmTag>(value);
}

UImmediate::Ptr VMUArg::uImm(VirtualMachine::Status value) {
  return std::make_shared<UImmStatus>(value);
}

ULocHeap::Ptr VMUArg::Heap(URegister::Ptr reg, int offset,
                           VirtualMachine::Type type) {
  return std::make_shared<ULocHeap>(reg, offset, type);
}

ULocFP::Ptr VMUArg::FP(int offset, VirtualMachine::Type type) {
  return std::make_shared<ULocFP>(offset, type);
}

ULocSP::Ptr VMUArg::SP(int offset, VirtualMachine::Type type) {
  return std::make_shared<ULocSP>(offset, type);
}

ULocHeap::Ptr VMUArg::HP(int offset, VirtualMachine::Type type) {
  return Heap(hp, offset, type);
}
