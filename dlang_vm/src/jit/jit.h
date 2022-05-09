// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

extern "C" {
#include <lightning.h>
}

// Static class providing utilities for common operations in JIT compilation
class JIT {
 protected:
  // State of the current JIT context
  inline static jit_state_t* _jit;
};
