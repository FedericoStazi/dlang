// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "runtime_system.h"

#include <iostream>

int RuntimeSystem::readInt() {
  int x;
  std::cout << "input> ";
  std::cin >> x;
  return x;
}
