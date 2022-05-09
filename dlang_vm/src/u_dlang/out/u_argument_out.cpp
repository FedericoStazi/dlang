// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_argument_out.h"

#include <string>

#include "../u_argument.h"
#include "../../virtual_machine/out/memory_out.h"
#include "../../virtual_machine/out/virtual_machine_out.h"

template<>
std::string Out::print(const UArgument::Ptr& arg) {
  return arg->print();
}

template<>
std::string Out::print(const UOperand::Ptr& arg) {
  return arg->print();
}

template<>
std::string Out::print(const URegister::Ptr& arg) {
  return arg->print();
}

template<>
std::string Out::print(const UImmediate::Ptr& arg) {
  return arg->print();
}

template<>
std::string Out::print(const ULocation::Ptr& arg) {
  return arg->print();
}

std::string URegister::print() const { return name_; }

std::string UImmInt::print() const { return Out::print(value_); }

std::string UImmTag::print() const { return Out::print(value_); }

std::string UImmStatus::print() const { return Out::print(value_); }

std::string ULocation::print() const {
  auto ptr = Out::print(getPtr());
  auto offset = ((getOffset() < 0) ? "" : "+") + Out::print(getOffset());
  auto type = Out::print(getType());
  return "(" + ptr + offset + ")->" + type;
}
