// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_instruction_out.h"

#include "../../out/out.h"
#include "../u_instruction.h"
#include "u_argument_out.h"
#include "../../virtual_machine/out/memory_out.h"
#include "../../virtual_machine/out/virtual_machine_out.h"
#include "../../virtual_machine/out/operations_out.h"
#include "../../data_structures/code.h"

template<>
std::string Out::print(const std::shared_ptr<UInstruction>& instruction) {
  return instruction->print();
}

std::string UGet::print() const {
  return Out::printSpaced("GET", a, b);
}

std::string USet::print() const {
  return Out::printSpaced("SET", a, b);
}

std::string UMove::print() const {
  return Out::printSpaced("MOVE", a, b);
}

std::string UUnary::print() const {
  return Out::printSpaced("UNARY", a, op, b);
}

std::string UOper::print() const {
  return Out::printSpaced("OPER", a, b, op, c);
}

std::string ULabel::print() const {
  return Out::printSpaced("LABEL", cp + 1);
}

std::string UGuard::print() const {
  return "GUARD";
}

std::string UMemCheck::print() const {
  return Out::printSpaced("MEM-CHECK", a);
}

std::string UTagCheck::print() const {
  if (tagA != tagB) {
    return Out::printSpaced("TAG-CHECK", tagA, "or", tagB, a);
  } else {
    return Out::printSpaced("TAG-CHECK", tagA, a);
  }
}

std::string UApply::print() const {
  return "APPLY";
}

std::string UReturn::print() const {
  return "RETURN";
}

std::string UHalt::print() const {
  return "HALT";
}

std::string UGoto::print() const {
  return Out::printSpaced("GOTO", destination);
}

std::string UBranch::print() const {
  return Out::printSpaced("BRANCH", destination, a);
}

template<>
std::string Out::print(const Code<UInstruction>& code) {
  std::string str;
  for (const auto& instruction : code) {
    str += print(2, instruction->cp) + ": " + print(instruction) + "\n";
  }
  return str + "\n";
}
