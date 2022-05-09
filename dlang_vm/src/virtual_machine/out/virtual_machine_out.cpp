// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "virtual_machine_out.h"

#include <memory>
#include <string>

#include "memory_out.h"

template<>
std::string Out::print(const std::shared_ptr<VirtualMachine>& vm) {
  std::string str;
  str += "Virtual Machine state:\n";
  str += printRow(7, printSpaced("SP: ", vm->sp),
                     printSpaced("FP: ", vm->fp),
                     printSpaced("CP: ", vm->cp),
                     printSpaced("HP: ", vm->hp)) + "\n";
  str += "STACK:\n";
  for (int i = 0; i < vm->sp; i++) {
    str += printSpaced("    ", i, ":", vm->stack.get(i).value,
                       " (" + print(vm->stack.get(i).tag) + ")") + "\n";
  }
  str += "HEAP:\n";
  for (int i = 0; i < vm->hp; i++) {
    str += printSpaced("    ", i, ":", vm->heap.get(i).value,
                       " (" + print(vm->heap.get(i).tag) + ")") + "\n";
  }
  str += "\n";
  return str;
}

template<>
std::string Out::print(const VirtualMachine::Status& state) {
  if (state == VirtualMachine::Status::Halted) { return "Halted"; }
  if (state == VirtualMachine::Status::Running) { return "Running"; }
  if (state == VirtualMachine::Status::RuntimeError) { return "RuntimeError"; }
  return "UNKOWNSTATE";
}

template<>
std::string Out::print(const VirtualMachine::Type& type) {
  if (type == VirtualMachine::Type::Tag) { return "tag"; }
  if (type == VirtualMachine::Type::Val) { return "val"; }
  return "UNKOWNTYPE";
}
