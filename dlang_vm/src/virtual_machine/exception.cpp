// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "exception.h"

Exception::Exception() : message_("Generic exception") {}

Exception::Exception(const std::string& message) : message_(message) {}

void Exception::setExceptionMessage(const std::string& message) {
  message_ = message;
}

const char* Exception::what() const noexcept {
  return message_.c_str();
}

UnkownInstruction::UnkownInstruction(std::string token,
                                     std::vector<std::string> instruction) {
  auto message = "Undefined token \"" + token + "\" in instruction \"";
  for (auto t : instruction) { message += t + " "; }
  message.back() = '"';
  setExceptionMessage(message);
}

RuntimeError::RuntimeError() : Exception("Runtime error") {}

InternalError::InternalError() : Exception("Internal error") {}

JITCompileError::JITCompileError() : Exception("JIT compilation error") {}

OptimizationError::OptimizationError() : Exception("Optimization error") {}
