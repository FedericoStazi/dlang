// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "b_group.h"

#pragma once

template <BGroup group>
bool BGroupRole<group>::isStart() const {
  return isStart_;
}

template <BGroup group>
bool BGroupRole<group>::isEnd() const {
  return isEnd_;
}

template <BGroup group>
bool BGroupRole<group>::isEntry() const {
  return isEntry_;
}

template <BGroup group>
bool BGroupRole<group>::isBeforeEntry() const {
  return isBeforeEntry_;
}

template <BGroup group>
StartOf<group>::StartOf() {
  BGroupRole<group>::isStart_ = true;
}

template <BGroup group>
EndOf<group>::EndOf() {
  BGroupRole<group>::isEnd_ = true;
}

template <BGroup group>
EntryOf<group>::EntryOf() {
  BGroupRole<group>::isEntry_ = true;
}

template <BGroup group>
BeforeEntryOf<group>::BeforeEntryOf() {
  BGroupRole<group>::isBeforeEntry_ = true;
}
