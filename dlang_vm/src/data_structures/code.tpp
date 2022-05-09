// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include "code.h"

template<typename T>
Code<T>::Code() = default;

template<typename T>
std::shared_ptr<T> Code<T>::getInstruction(int index) const {
  return instructions_.at(index);
}

template<typename T>
size_t Code<T>::size() const {
  return instructions_.size();
}

template<typename T>
auto Code<T>::begin() const {
  return instructions_.begin();
}

template<typename T>
auto Code<T>::end() const {
  return instructions_.end();
}

template<typename T>
void Code<T>::add(std::shared_ptr<T> instruction) {
  instructions_.push_back(instruction);
}

template<typename T>
void Code<T>::add(const std::shared_ptr<Code<T>>& code)  {
  for (const auto& instruction : *code) {
    add(instruction);
  }
}

template<typename T>
const Code<T>& Code<T>::operator+(const Code<T>& code) {
  for (const auto& instruction : code) {
    add(instruction);
  }
  return *this;
}

template<typename T>
const Code<T>& Code<T>::operator+=(const Code<T>& code) {
  for (const auto& instruction : code) {
    add(instruction);
  }
  return *this;
}
