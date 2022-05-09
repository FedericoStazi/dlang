// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "group_jit.h"

template<BGroup group>
GroupJIT<group>::GroupJIT(size_t jitThreshold) : jitThreshold_(jitThreshold) {}

template<BGroup group>
void GroupJIT<group>::notifyLanding(size_t cp) {
  if (cp > landings_.size()) {
    landings_.resize(2 * cp);
  }
  landings_[cp]++;
}

template<BGroup group>
void GroupJIT<group>::notifyRunJIT(size_t cp) {}

template<BGroup group>
JITSequence GroupJIT<group>::makeJITSequence(const Code<BInstruction>& code,
                                             size_t startCp) {
  // Check if all compilation conditions are met
  if (landings_[startCp] < jitThreshold_ ||
      !code.getInstruction(startCp)->BGroupRole<group>::isStart()) {
    return {};
  }

  // Get the range of cps
  JITSequence::Cps cps;
  cps.push_back(startCp);
  while (!code.getInstruction(cps.back())->BGroupRole<group>::isEnd()) {
    cps.push_back(cps.back() + 1);
  }

  // Get the entry points to the jit compiled code (and their lengths)
  JITSequence::EntryPoints entryPoints;
  for (auto cp : cps) {
    if (code.getInstruction(cp)->BGroupRole<group>::isEntry()) {
      entryPoints.emplace_back(cp, cps.back() - cp + 1);
    }
    if (code.getInstruction(cp)->BGroupRole<group>::isBeforeEntry()) {
      entryPoints.emplace_back(cp + 1, cps.back() - (cp + 1) + 1);
    }
  }

  JITSequence jitSequence{cps, entryPoints};
  if (group == Function) { jitSequence.setFunction(); }
  return jitSequence;
}
