// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <string>
#include <vector>

class Exception : public std::exception {
 public:
  Exception();
  explicit Exception(const std::string& message);
  void setExceptionMessage(const std::string& message);
  virtual const char* what() const noexcept;
 private:
  std::string message_;
};

// Thrown when the program contains an unkown instruction
class UnkownInstruction : public Exception {
 public:
  // token is the part of instruction that caused the error
  UnkownInstruction(std::string token, std::vector<std::string> instruction);
};

// Thrown when there an instruction is missing an "if" statement
class InternalError : public Exception {
 public:
  InternalError();
};

// Thrown when there is an error accessing memory (e.g. out of bounds)
class RuntimeError : public Exception {
 public:
  RuntimeError();
};

// Thrown when there is an error during jit compilation
class JITCompileError : public Exception {
 public:
  JITCompileError();
};

// Thrown when there is an error during optimization
class OptimizationError : public Exception {
 public:
  OptimizationError();
};
