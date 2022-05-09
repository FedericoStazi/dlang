// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>

#include "../../out/out.h"
#include "../u_instruction.h"
#include "../../data_structures/code.h"

template<>
std::string Out::print(const std::shared_ptr<UInstruction>& instruction);

template<>
std::string Out::print(const Code<UInstruction>& code);
