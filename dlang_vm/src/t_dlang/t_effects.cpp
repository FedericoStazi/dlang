// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "t_effects.h"

#include <memory>
#include <utility>

TEffects TEffects::makeWrite(TVariable::Ptr var) {
  TEffects effects;
  effects.writeVar_ = var;
  return effects;
}

TEffects TEffects::makeTagCheck(TArgument::Ptr arg, Tag tagA, Tag tagB) {
  TEffects effects;
  std::pair<Tag, Tag> p = {tagA, tagB};
  effects.tagCheck_ = std::make_shared<Check>(arg, p);
  return effects;
}

TEffects TEffects::makeBoundsCheck(TArgument::Ptr arg) {
  TEffects effects;
  effects.boundsCheckArg_ = arg;
  return effects;
}

TEffects TEffects::makeOther() {
  return TEffects();
}

TVariable::Ptr TEffects::getWrite() {
  return writeVar_;
}

std::shared_ptr<TEffects::Check> TEffects::getTagCheck() {
  return tagCheck_;
}

TArgument::Ptr TEffects::getMemCheck() {
  return boundsCheckArg_;
}

bool TEffects::getOther() {
  return !(writeVar_ || tagCheck_ || boundsCheckArg_);
}
