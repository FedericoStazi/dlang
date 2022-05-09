// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <string>

#include "../../out/out.h"
#include "../u_argument.h"

template<>
std::string Out::print(const UArgument::Ptr& arg);

template<>
std::string Out::print(const UOperand::Ptr& arg);

template<>
std::string Out::print(const URegister::Ptr& arg);

template<>
std::string Out::print(const UImmediate::Ptr& arg);

template<>
std::string Out::print(const ULocation::Ptr& arg);
