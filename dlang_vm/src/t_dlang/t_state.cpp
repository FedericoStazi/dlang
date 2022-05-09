// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "t_state.h"
#include "../u_dlang/u_argument.h"
#include "../virtual_machine/exception.h"

void TState::updateSp(int val) {
  sp_ += val;
}

void TState::resetSP() {
  if (!isFunction_) {
    sp_ = 0;
    uSPLocations_.clear();
  }
}

void TState::resetFP() {
  if (!isFunction_) {
    uFPLocations_.clear();
  }
}

std::shared_ptr<TImmediate>
    TState::makeTArgument(std::shared_ptr<UImmediate> uImm) {
  return std::make_shared<TImmediate>(uImm);
}

std::shared_ptr<TVariable>
    TState::makeTArgument(std::shared_ptr<URegister> uReg) {
  if (!uRegisters_.count(uReg->getReg())) {
    uRegisters_.insert({uReg->getReg(), std::make_shared<TVariable>(uReg)});
  }
  return uRegisters_.at(uReg->getReg())->copy(uReg);
}

std::shared_ptr<TVariable> TState::makeTArgument(std::shared_ptr<ULocSP> uLoc) {
  int id = 2 * (sp_ + uLoc->getOffset()) + static_cast<int>(uLoc->getType());
  if (!uSPLocations_.count(id)) {
    uSPLocations_.insert({id, std::make_shared<TVariable>(uLoc)});
  }
  return uSPLocations_.at(id)->copy(uLoc);
}

std::shared_ptr<TVariable> TState::makeTArgument(std::shared_ptr<ULocFP> uLoc) {
  int id = 2 * uLoc->getOffset() + static_cast<int>(uLoc->getType());
  if (!uFPLocations_.count(id)) {
    uFPLocations_.insert({id, std::make_shared<TVariable>(uLoc)});
  }
  return uFPLocations_.at(id)->copy(uLoc);
}

std::shared_ptr<TVariable>
    TState::makeTArgument(std::shared_ptr<ULocHeap> uLoc) {
  return std::make_shared<TVariable>(uLoc);
}

void TState::addBranchDestination(size_t destination) {
  if (isFunction_) {
    if (labelOffsets_.count(destination) && labelOffsets_[destination] != sp_) {
      throw OptimizationError();
    }
    labelOffsets_[destination] = sp_;
  }
}

void TState::addLabel(size_t cp) {
  if (isFunction_) {
    if (labelOffsets_.count(cp)) {
      sp_ = labelOffsets_[cp];
    } else {
      labelOffsets_[cp] = sp_;
    }
  }
}

void TState::setFunction() {
  isFunction_ = true;
}

bool TState::isFunction() const {
  return isFunction_;
}
