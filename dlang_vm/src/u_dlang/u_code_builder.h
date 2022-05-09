// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

#include "u_instruction.h"
#include "../data_structures/code.h"

class UCodeBuilder {
 public:
  UCodeBuilder(size_t cp, std::shared_ptr<VirtualMachine> vm);

  template<typename T, typename... Args>
  UCodeBuilder& add(const Args&... args);

  template<typename T, typename... Args>
  UCodeBuilder& addIf(bool condition, const Args&... args);

  operator Code<UInstruction>() const;

 private:
  const size_t cp_;
  std::shared_ptr<VirtualMachine> vm_;
  Code<UInstruction> code_;
};

#include "u_code_builder.tpp"
