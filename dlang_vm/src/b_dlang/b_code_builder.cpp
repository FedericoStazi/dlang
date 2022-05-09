// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "b_code_builder.h"

#include <memory>
#include <sstream>
#include <vector>

Code<BInstruction> BCodeBuilder::fromString(std::string codeString) {
  Code<BInstruction> code;
  std::stringstream codeStringStream;
  std::string instructionString;
  std::map<std::string, size_t> labels;

  // Iterate over all input lines to get the labels' positions
  codeStringStream = std::stringstream(codeString);
  for (size_t codeIndex = 0;
       getline(codeStringStream, instructionString, '\n');
       codeIndex++) {
    if (instructionString.substr(0, 5) == "LABEL") {
      labels.insert({instructionString.substr(6), codeIndex});
    }
    if (instructionString.substr(0, 8) == "FUNCTION") {
      labels.insert({instructionString.substr(9), codeIndex});
    }
  }

  // Iterate over all input lines to create the vector of instructions
  codeStringStream = std::stringstream(codeString);
  while (getline(codeStringStream, instructionString, '\n')) {
    // Iterate over all (space-separated) tokens
    std::vector<std::string> tokens;
    std::stringstream instructionStringStream(instructionString);
    std::string token;
    while (getline(instructionStringStream, token, ' ')) {
      tokens.push_back(token);
    }
    if (!tokens.empty()) {
      code.add(getInstructionFromTokens(tokens, code.size(), labels));
    }
  }
  return code;
}

std::shared_ptr<BInstruction> BCodeBuilder::getInstructionFromTokens(
    std::vector<std::string> tokens, size_t cp,
    const std::map<std::string, size_t>& labels) {
  if (tokens.at(0) == "UNARY") {
    if (tokens.at(1) == "NOT") {
      return std::make_shared<BUnary>(cp, BUnary::Op::Not);
    } else if (tokens.at(1) == "NEG") {
      return std::make_shared<BUnary>(cp, BUnary::Op::Neg);
    } else if (tokens.at(1) == "READ") {
      return std::make_shared<BUnary>(cp, BUnary::Op::Read);
    } else {
      throw UnkownInstruction(tokens.at(1), tokens);
    }
  } else if (tokens.at(0) == "OPER") {
    if (tokens.at(1) == "AND") {;
      return std::make_shared<BOper>(cp, BOper::Op::And);
    } else if (tokens.at(1) == "OR") {;
      return std::make_shared<BOper>(cp, BOper::Op::Or);
    } else if (tokens.at(1) == "EQ") {;
      return std::make_shared<BOper>(cp, BOper::Op::Eq);
    } else if (tokens.at(1) == "LT") {;
      return std::make_shared<BOper>(cp, BOper::Op::Lt);
    } else if (tokens.at(1) == "ADD") {;
      return std::make_shared<BOper>(cp, BOper::Op::Add);
    } else if (tokens.at(1) == "SUB") {;
      return std::make_shared<BOper>(cp, BOper::Op::Sub);
    } else if (tokens.at(1) == "MUL") {;
      return std::make_shared<BOper>(cp, BOper::Op::Mul);
    } else if (tokens.at(1) == "DIV") {;
      return std::make_shared<BOper>(cp, BOper::Op::Div);
    } else {
      throw UnkownInstruction(tokens.at(1), tokens);
    }
  } else if (tokens.at(0) == "MK_PAIR") {
    return std::make_shared<BMkPair>(cp);
  } else if (tokens.at(0) == "FST") {
    return std::make_shared<BFst>(cp);
  } else if (tokens.at(0) == "SND") {
    return std::make_shared<BSnd>(cp);
  } else if (tokens.at(0) == "MK_INL") {
    return std::make_shared<BMkInl>(cp);
  } else if (tokens.at(0) == "MK_INR") {
    return std::make_shared<BMkInr>(cp);
  } else if (tokens.at(0) == "PUSH") {
    if (tokens.at(1) == "STACK_UNIT") {
      return std::make_shared<BPush>(cp, BPush::Tag::Unit, 0);
    } else if (tokens.at(1) == "STACK_BOOL") {
      return std::make_shared<BPush>(cp, BPush::Tag::Bool,
                                     tokens.at(2) == "true");
    } else if (tokens.at(1) == "STACK_INT") {
      return std::make_shared<BPush>(cp, BPush::Tag::Int,
                                     std::stoul(tokens.at(2)));
    } else {
      throw UnkownInstruction(tokens.at(1), tokens);
    }
  } else if (tokens.at(0) == "APPLY") {
    return std::make_shared<BApply>(cp);
  } else if (tokens.at(0) == "LOOKUP") {
    if (tokens.at(1) == "STACK_LOCATION") {
      return std::make_shared<BLookup>(cp, BLookup::Location::Stack,
                                       std::stoul(tokens.at(2)));
    } else if (tokens.at(1) == "HEAP_LOCATION") {
      return std::make_shared<BLookup>(cp, BLookup::Location::Heap,
                                       std::stoul(tokens.at(2)));
    } else {
      throw UnkownInstruction(tokens.at(1), tokens);
    }
  } else if (tokens.at(0) == "RETURN") {
    return std::make_shared<BReturn>(cp);
  } else if (tokens.at(0) == "MK_CLOSURE") {
    return std::make_shared<BMkClosure>(cp, labels.at(tokens.at(1)),
                                        std::stoul(tokens.at(2)));
  } else if (tokens.at(0) == "SWAP") {
    return std::make_shared<BSwap>(cp);
  } else if (tokens.at(0) == "POP") {
    return std::make_shared<BPop>(cp);
  } else if (tokens.at(0) == "LABEL") {
    return std::make_shared<BLabel>(cp);
  } else if (tokens.at(0) == "FUNCTION") {
    return std::make_shared<BFunction>(cp);
  } else if (tokens.at(0) == "DEREF") {
    return std::make_shared<BDeref>(cp);
  } else if (tokens.at(0) == "MK_REF") {
    return std::make_shared<BMkRef>(cp);
  } else if (tokens.at(0) == "ASSIGN") {
    return std::make_shared<BAssign>(cp);
  } else if (tokens.at(0) == "HALT") {
    return std::make_shared<BHalt>(cp);
  } else if (tokens.at(0) == "GOTO") {
    return std::make_shared<BGoto>(cp, labels.at(tokens.at(1)));
  } else if (tokens.at(0) == "TEST") {
    return std::make_shared<BTest>(cp, labels.at(tokens.at(1)));
  } else if (tokens.at(0) == "CASE") {
    return std::make_shared<BCase>(cp, labels.at(tokens.at(1)));
  } else {
    throw UnkownInstruction(tokens.at(0), tokens);
  }
}
