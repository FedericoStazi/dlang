// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "t_instruction_out.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "t_argument_out.h"
#include "../../virtual_machine/out/memory_out.h"
#include "../../virtual_machine/out/operations_out.h"
#include "../../u_dlang/u_instruction.h"

template<>
std::string Out::print(const std::shared_ptr<TInstruction>& instruction) {
  if (!instruction) { return "OUT (cp = -1)"; }
  return instruction->print();
}

template<>
std::string Out::print(const Code<TInstruction>& code) {
  std::string str;
  for (const auto& instruction : code) {
    str += print(2, instruction->getUInstruction()->cp)
        + ": " + print(instruction) + "\n";
  }
  return str + "\n";
}

template<>
std::string Out::print(const FlowGraph<TInstruction>& graph) {
  using TNodePtr = FlowGraph<TInstruction>::NodePtr;

  // Visit and assign an integer id to all nodes
  std::unordered_map<TNodePtr, size_t> nodeId;
  std::function<void(TNodePtr)> assignNodeId = [&](TNodePtr node) {
    if (!node || nodeId.count(node)) { return; }
    nodeId[node] = nodeId.size() + 1;
    for (auto nextNode : node->getSuccessors()) {
      assignNodeId(nextNode);
    }
  };
  assignNodeId(graph.getStartNode());

  // Visit and print all nodes
  std::unordered_set<TNodePtr> visited;
  static int blockIdx = 0;
  std::function<std::string(TNodePtr)> printNodes = [&](TNodePtr node) {
    if (!node || visited.count(node)) { return print(""); }
    visited.insert(node);
    std::string str;
    std::string block;
    if (node->getPredecessors().size()) {
      block += "START" + Out::print(++blockIdx) + " ";
    }
    if (node->getSuccessors().size() != 1 || (!node->getSuccessors().at(0))) {
      block += "END" + Out::print(blockIdx) + " ";
    }
    str += printSpaced("  Node", nodeId.at(node), ":",
                       block, node->getValue()) + "\n";
    for (auto nextNode : node->getSuccessors()) {
      str += printSpaced("    --->", nodeId.at(nextNode)) + "\n";
    }
    for (auto nextNode : node->getSuccessors()) {
      str += printNodes(nextNode);
    }
    return str;
  };
  return "Flow graph:\n" + printNodes(graph.getStartNode()) + "\n";
}

std::string TMove::print() const { return Out::printSpaced("MOVE", a, b); }

std::string TUnary::print() const {
  auto op = std::dynamic_pointer_cast<UUnary>(getUInstruction())->op;
  return Out::printSpaced("UNARY", a, op, b);
}

std::string TOper::print() const {
  auto op = std::dynamic_pointer_cast<UOper>(getUInstruction())->op;
  return Out::printSpaced("OPER", a, b, op, c);
}

std::string TLabel::print() const {
  return "LABEL";
}

std::string TGuard::print() const {
  return "GUARD";
}

std::string TMemCheck::print() const {
  return Out::printSpaced("MEM-CHECK", a);
}

std::string TTagCheck::print() const {
  if (tagA != tagB) {
    return Out::printSpaced("TAG-CHECK", tagA, tagB, a);
  } else {
    return Out::printSpaced("TAG-CHECK", tagA, a);
  }
}

std::string TApply::print() const {
  return isFunction() ? Out::printSpaced(
      "APPLY", "arg:", argTag, argVal, "clo:", cloTag, cloVal,
      "fp:", fpTag, fpVal, "cp:", cpTag, cpVal)
      : "APPLY";
}

std::string TReturn::print() const {
  return isFunction() ? Out::printSpaced("RETURN", aTag, aVal) : "RETURN";
}

std::string THalt::print() const {
  return Out::printSpaced("HALT");
}

std::string TGoto::print() const {
  return Out::printSpaced("GOTO");
}

std::string TBranch::print() const {
  return Out::printSpaced("BRANCH", a);
}
