// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "jit_sequence.h"

JITSequence::JITSequence(Cps cps, EntryPoints entryPoints)
    : cps_(cps), entryPoints_(entryPoints) {}

bool JITSequence::isEmpty() const {
  return !cps_.has_value();
}

const JITSequence::Cps JITSequence::getCps() const {
  return *cps_;
}

const JITSequence::EntryPoints JITSequence::getEntryPoints() const {
  return *entryPoints_;
}

void JITSequence::setFunction() {
  isFunction_ = true;
}

bool JITSequence::isFunction() const {
  return isFunction_;
}
