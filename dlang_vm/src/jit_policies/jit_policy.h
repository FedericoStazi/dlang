// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "jit_sequence.h"
#include "../b_dlang/b_instruction.h"
#include "../data_structures/code.h"

class JITPolicy {
 public:
  virtual void notifyLanding(size_t cp) = 0;
  virtual void notifyRunJIT(size_t cp) = 0;
  virtual JITSequence makeJITSequence(const Code<BInstruction>& code,
                                      size_t startCp) = 0;
};
