// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

template<typename T>
std::string Out::print(const T& x) {
  if constexpr (std::is_arithmetic_v<T>) {
  return std::to_string(x);
  } else {
    return x;
  }
}

template<typename T>
std::string Out::print(int width, const T& x) {
  auto str = Out::print(x);
  if (width - static_cast<int>(str.size()) > 0) {
    str += std::string(width - str.size(), ' ');
  }
  return str;
}

template<typename T, typename... Ts>
std::string Out::printSpaced(const T& x, const Ts& ...xs) {
  return Out::print(x) + " " + printSpaced(xs...);
}

template<typename T>
std::string Out::printSpaced(const T& x) {
  return Out::print(x);
}

template<typename T, typename... Ts>
std::string Out::printRow(int width, const T& x, const Ts& ...xs) {
  return Out::print(width, x) + Out::printRow(width, xs...);
}

template<typename T>
std::string Out::printRow(int width, const T& x) {
  return Out::print(width, x);
}
