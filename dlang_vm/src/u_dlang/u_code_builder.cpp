// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_code_builder.h"

#include <memory>

UCodeBuilder::UCodeBuilder(size_t cp, std::shared_ptr<VirtualMachine> vm)
    : cp_(cp), vm_(vm) {}

UCodeBuilder::operator Code<UInstruction>() const {
  return code_;
}
