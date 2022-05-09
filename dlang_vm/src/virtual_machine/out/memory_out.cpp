// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "memory_out.h"

#include <string>

template<>
std::string Out::print(const Tag& tag) {
  if (tag == Unit) { return "Unit"; }
  if (tag == Bool) { return "Bool"; }
  if (tag == Int) { return "Int"; }
  if (tag == HeapIndex) { return "HeapIndex"; }
  if (tag == HeapRef) { return "HeapRef"; }
  if (tag == CodeIndex) { return "CodeIndex"; }
  if (tag == ReturnAddress) { return "ReturnAddress"; }
  if (tag == FramePointer) { return "FramePointer"; }
  if (tag == PairHeader) { return "PairHeader"; }
  if (tag == InlHeader) { return "InlHeader"; }
  if (tag == InrHeader) { return "InrHeader"; }
  if (tag == ClosureHeader) { return "ClosureHeader"; }
  return "UNKNOWNTAG";
}
