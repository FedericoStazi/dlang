// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <unordered_set>
#include <vector>

#include "jit_policy.h"
#include "../data_structures/int_set.h"

class TracingJIT : public JITPolicy {
 public:
  explicit TracingJIT(size_t jitThreshold, size_t maxLength = 256);
  virtual void notifyLanding(size_t cp);
  virtual void notifyRunJIT(size_t cp);
  virtual JITSequence makeJITSequence(const Code<BInstruction>& code,
                                      size_t startCp);
 private:
  std::vector<size_t> trace_;
  std::vector<size_t> landings_{256};
  size_t prevCp_ = 0;
  const size_t jitThreshold_;
  const size_t maxLength_;
  size_t skipCnt_ = 0;
};
