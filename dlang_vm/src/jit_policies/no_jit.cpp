// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "no_jit.h"

void NoJIT::notifyLanding(size_t cp) {}
void NoJIT::notifyRunJIT(size_t cp) {}
JITSequence NoJIT::makeJITSequence(const Code<BInstruction>& code,
                                   size_t startCp) { return {}; }
