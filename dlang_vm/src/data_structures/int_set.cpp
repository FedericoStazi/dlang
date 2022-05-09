// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "int_set.h"

void IntSet::insert(const IntSet& other) {
  if (other.set_.size() > set_.size()) {
    set_.resize(other.set_.size());
  }
  for (size_t i = 0; i < other.set_.size(); i++) {
    set_[i] |= other.set_[i];
  }
}

void IntSet::insert(size_t x) {
  if ((x / kNumBits) >= set_.size()) {
    set_.resize(1 + (x / kNumBits));
  }
  set_[x / kNumBits] |= (1ul << (x & (kNumBits - 1)));
}

void IntSet::erase(size_t x) {
  if ((x / kNumBits) < set_.size()) {
    set_[x / kNumBits] &= (-1ul) ^ (1ul << (x & (kNumBits - 1)));
  }
}

bool IntSet::get(size_t x) const {
  if ((x / kNumBits) >= set_.size()) {
    return false;
  }
  return set_[x / kNumBits] & (1ul << (x & (kNumBits - 1)));
}

bool IntSet::operator!=(const IntSet& other) const {
  return set_ != other.set_;
}
