// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

// A group of jit compiled instructions
class CompiledInstructions {
 public:
  explicit CompiledInstructions(void* compiled_function);
  void run();

 private:
  typedef void (*VoidFunction)();
  VoidFunction compiledFunction_;
};
