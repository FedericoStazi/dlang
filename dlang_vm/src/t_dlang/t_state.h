// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "t_argument.h"
#include "../jit/jit.h"

class TImmediate;
class TVariable;

class URegister;
class UImmediate;
class ULocSP;
class ULocFP;
class ULocHeap;

class TState {
 public:
  void updateSp(int val);
  void resetSP();
  void resetFP();

  std::shared_ptr<TImmediate> makeTArgument(std::shared_ptr<UImmediate> uImm);
  std::shared_ptr<TVariable> makeTArgument(std::shared_ptr<URegister> uReg);
  std::shared_ptr<TVariable> makeTArgument(std::shared_ptr<ULocSP> uLoc);
  std::shared_ptr<TVariable> makeTArgument(std::shared_ptr<ULocFP> uLoc);
  std::shared_ptr<TVariable> makeTArgument(std::shared_ptr<ULocHeap> uLoc);

  void addBranchDestination(size_t destination);
  void addLabel(size_t cp);

  void setFunction();
  bool isFunction() const;

 private:
  int sp_ = 0;
  std::unordered_map<jit_reg_t, std::shared_ptr<TVariable>> uRegisters_;
  std::unordered_map<int, std::shared_ptr<TVariable>> uSPLocations_;
  std::unordered_map<int, std::shared_ptr<TVariable>> uFPLocations_;
  std::unordered_map<size_t, int> labelOffsets_;
  bool isFunction_ = false;
};
