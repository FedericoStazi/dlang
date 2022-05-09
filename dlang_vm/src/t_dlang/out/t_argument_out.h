// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <string>

#include "../../out/out.h"
#include "../t_argument.h"

template<>
std::string Out::print(const TArgument::Ptr& arg);

template<>
std::string Out::print(const TVariable::Ptr& arg);
