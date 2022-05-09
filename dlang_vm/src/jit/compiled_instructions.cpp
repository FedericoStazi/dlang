// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "compiled_instructions.h"

CompiledInstructions::CompiledInstructions(void* compiledFunction)
    : compiledFunction_((VoidFunction) compiledFunction) {}

void CompiledInstructions::run() {
  compiledFunction_();
}
