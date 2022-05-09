// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "t_argument.h"

TArgument::TArgument(UArgPtr uArgument) : uArgument_(uArgument) {}

TArgument::UArgPtr TArgument::getUArgument() const {
  return uArgument_;
}

TImmediate::TImmediate(UArgPtr uArgument) : TArgument(uArgument) {}

std::string TImmediate::getName() const {
  return "imm";
}

TVariable::TVariable(UArgPtr uArgument)
    : TArgument(uArgument), uid_(nextUid()) {}

TVariable::TVariable(UArgPtr uArgument, size_t uid)
    : TArgument(uArgument), uid_(uid) {}

std::string TVariable::getName() const {
  return "x" + std::to_string(uid_);
}

std::shared_ptr<TVariable> TVariable::copy(UArgPtr uArgument) {
  return std::make_shared<TVariable>(uArgument, uid_);
}

bool TVariable::operator==(const TVariable& other) {
  return uid_ == other.uid_;
}

size_t TVariable::getUID() const {
  return uid_;
}

size_t TVariable::nextUid() {
  return ++nextUid_;
}
