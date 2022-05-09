// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>

#include "../../out/out.h"
#include "../virtual_machine.h"

template<>
std::string Out::print(const std::shared_ptr<VirtualMachine>&);

template<>
std::string Out::print(const VirtualMachine::Status&);

template<>
std::string Out::print(const VirtualMachine::Type&);
