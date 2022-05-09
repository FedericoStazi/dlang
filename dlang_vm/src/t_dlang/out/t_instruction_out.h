// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>

#include "../../out/out.h"
#include "../t_instruction.h"
#include "../../data_structures/flow_graph.h"
#include "../../data_structures/code.h"

template<>
std::string Out::print(const std::shared_ptr<TInstruction>& instruction);

template<>
std::string Out::print(const Code<TInstruction>& code);

template<>
std::string Out::print(const FlowGraph<TInstruction>& graph);
