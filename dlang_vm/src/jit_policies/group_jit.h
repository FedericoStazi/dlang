// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <vector>

#include "jit_policy.h"

template<BGroup group>
class GroupJIT : public JITPolicy {
 public:
  explicit GroupJIT(size_t jitThreshold);

  virtual void notifyLanding(size_t cp);
  virtual void notifyRunJIT(size_t cp);
  virtual JITSequence makeJITSequence(const Code<BInstruction>& code,
                                      size_t startCp);

 private:
  size_t jitThreshold_;
  std::vector<size_t> landings_{256};
};

#include "group_jit.tpp"
