// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "operations_out.h"

#include <string>

template<>
std::string Out::print(const UnaryOp& op) {
  if (op == Not) { return "!"; }
  if (op == Neg) { return "-"; }
  if (op == Read) { return "?"; }
  return "UNKNOWNOP";
}

template<>
std::string Out::print(const BinaryOp& op) {
  if (op == And) { return "&"; }
  if (op == Or) { return "|"; }
  if (op == Eq) { return "="; }
  if (op == Lt) { return "<"; }
  if (op == Add) { return "+"; }
  if (op == Sub) { return "-"; }
  if (op == Mul) { return "*"; }
  if (op == Div) { return "/"; }
  return "UNKNOWNOP";
}
