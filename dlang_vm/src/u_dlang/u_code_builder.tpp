// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "u_code_builder.h"

template<typename T, typename... Args>
UCodeBuilder& UCodeBuilder::UCodeBuilder::add(const Args&... args) {
  code_.add(std::make_shared<T>(cp_, vm_, args...));
  return *this;
}

template<typename T, typename... Args>
UCodeBuilder& UCodeBuilder::addIf(bool condition, const Args&... args) {
  if (condition) {
    code_.add(std::make_shared<T>(cp_, vm_, args...));
   }
  return *this;
}
