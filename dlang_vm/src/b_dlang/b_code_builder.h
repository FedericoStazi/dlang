// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "b_instruction.h"
#include "../data_structures/code.h"

class BCodeBuilder {
 public:
  static Code<BInstruction> fromString(std::string codeString);

 private:
  static std::shared_ptr<BInstruction> getInstructionFromTokens(
      std::vector<std::string> tokens, size_t cp,
      const std::map<std::string, size_t>& labels);
};
