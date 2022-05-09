// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "jit_policy.h"

class NoJIT : public JITPolicy {
 public:
  virtual void notifyLanding(size_t cp);
  virtual void notifyRunJIT(size_t cp);
  virtual JITSequence makeJITSequence(const Code<BInstruction>& code,
                                      size_t startCp);
};
