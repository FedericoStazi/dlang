// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "t_argument_out.h"

#include <string>

template<>
std::string Out::print(const TArgument::Ptr& arg) {
  return arg->getName();
}

template<>
std::string Out::print(const TVariable::Ptr& arg) {
  return arg->getName();
}
