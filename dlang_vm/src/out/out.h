// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <algorithm>
#include <string>

class Out {
 public:
  template<typename T>
  static std::string print(const T& x);

  template<typename T>
  static std::string print(int width, const T& x);

  template<typename T, typename... Ts>
  static std::string printSpaced(const T& x, const Ts& ...xs);

  template<typename T>
  static std::string printSpaced(const T& x);

  template<typename T, typename... Ts>
  static std::string printRow(int width, const T& x, const Ts& ...xs);

  template<typename T>
  static std::string printRow(int width, const T& x);
};

#include "out.tpp"
