// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "virtual_machine.h"

#include <string>

#include "exception.h"

std::string VirtualMachine::toString(Item item) {
  if (item.tag == Tag::Unit) {
    return "()";
  } else if (item.tag == Tag::Bool) {
    return item.value ? "true" : "false";
  } else if (item.tag == Tag::Int) {
    return std::to_string(static_cast<int>(item.value));
  } else if (item.tag == Tag::HeapRef) {
    return "reference";
  } else if (item.tag == Tag::HeapIndex) {
    try {
      heap.checkTag(item.value, Tag::PairHeader);
    } catch (const ::RuntimeError&) {
      return "ERROR";
    }
    auto l = toString(heap.get(item.value + 1));
    auto r = toString(heap.get(item.value + 2));
    if (l == "ERROR" || r == "ERROR") { return "ERROR"; }
    return "(" + l + ", " + r + ")";
  } else {
    return "ERROR";
  }
}

std::string VirtualMachine::getResult() {
  if (sp <= 0) { return "ERROR"; }
  return toString(stack.get(sp - 1));
}
