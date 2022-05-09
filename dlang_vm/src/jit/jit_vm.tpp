// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "jit_vm.h"

template<typename FuncType, typename... ArgsTypes>
void JITVM::call(jit_reg_t reg, FuncType func, ArgsTypes... args) {
  // Save caller-saved registers
  jit_sti(&spillR0, JIT_R0);
  jit_sti(&spillR1, JIT_R1);
  jit_sti(&spillR2, JIT_R2);

  // Prepare arguments
  jit_prepare();
  (jit_pushargi(reinterpret_cast<jit_word_t>(args)), ...);

  // Call function and store return value
  jit_finishi(reinterpret_cast<void*>(func));
  jit_retval(reg);

  // Restore caller-saved registers (unless the register has the result)
  if (reg != JIT_R0) jit_ldi(JIT_R0, &spillR0);
  if (reg != JIT_R1) jit_ldi(JIT_R1, &spillR1);
  if (reg != JIT_R2) jit_ldi(JIT_R2, &spillR2);
}
