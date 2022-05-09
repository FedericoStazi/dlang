// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "../jit/jit.h"
#include "../virtual_machine/virtual_machine.h"

class JITVM : public JIT {
 public:
  // sp, fp, cp, hp are saved in registers for JIT compiled code
  static constexpr jit_reg_t sp  = JIT_V0;
  static constexpr jit_reg_t fp  = JIT_V1;
  static constexpr jit_reg_t cp  = JIT_V2;
  static constexpr jit_reg_t hp  = JIT_V3;
  static constexpr jit_reg_t tmp = JIT_V4;

  template<typename FuncType, typename... ArgsTypes>
  static void call(jit_reg_t reg, FuncType func, ArgsTypes... args);

 private:
  inline static size_t spillR0, spillR1, spillR2;
};

#include "jit_vm.tpp"
