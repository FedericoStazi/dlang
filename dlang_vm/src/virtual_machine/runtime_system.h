// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

// Static class providing some functions needed by the vm at runtime
class RuntimeSystem {
 public:
  RuntimeSystem() = delete;  // Static class
  // Get an integer from user input
  static int readInt();
};
