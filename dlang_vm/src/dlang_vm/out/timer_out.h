// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <string>

#include "../../out/out.h"
#include "../timer.h"

template<>
std::string Out::print(const Timer&);
