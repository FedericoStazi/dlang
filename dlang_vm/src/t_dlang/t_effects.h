// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <utility>

#include "t_argument.h"
#include "../virtual_machine/memory.h"

class TEffects {
 public:
  using Check = std::pair<TArgument::Ptr, std::pair<Tag, Tag>>;

  // Static methods to create the object
  static TEffects makeWrite(TVariable::Ptr var);
  static TEffects makeTagCheck(TArgument::Ptr arg, Tag tagA, Tag tagB);
  static TEffects makeBoundsCheck(TArgument::Ptr arg);
  static TEffects makeOther();

  // Methods to get the side effect
  TVariable::Ptr getWrite();
  std::shared_ptr<Check> getTagCheck();
  TArgument::Ptr getMemCheck();
  bool getOther();

 private:
  TEffects() = default;
  TVariable::Ptr writeVar_;
  std::shared_ptr<Check> tagCheck_;
  TArgument::Ptr boundsCheckArg_;
};
