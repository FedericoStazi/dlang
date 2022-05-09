// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "tracing_jit.h"

TracingJIT::TracingJIT(size_t jitThreshold, size_t maxLength)
    : jitThreshold_(jitThreshold), maxLength_(maxLength) {}

void TracingJIT::notifyLanding(size_t cp) {
  if (trace_.empty()) {
    if (cp < prevCp_) {
      if (cp > landings_.size()) {
        landings_.resize(2 * cp);
      }
      landings_[cp]++;
      if (landings_[cp] >= jitThreshold_) {
        trace_.push_back(cp);
        skipCnt_ = 0;
      }
    }
  } else if (skipCnt_) {
    skipCnt_++;
  } else {
    trace_.push_back(cp);
  }

  prevCp_ = cp;
}

void TracingJIT::notifyRunJIT(size_t cp) {
  if (!skipCnt_) {
    if (trace_.size() == 1) {
      trace_.clear();
    } else {
      skipCnt_ = 1;
    }
  }
  prevCp_ = 1'000'000'000;
}

JITSequence TracingJIT::makeJITSequence(const Code<BInstruction>& code,
                                        size_t cp) {
  if (trace_.size() + skipCnt_ >= maxLength_) {
    trace_.clear();
    skipCnt_ = 0;
    return {};
  }

  if (trace_.size() + skipCnt_ > 1 && trace_.front() == cp) {
    std::vector<size_t> traceCopy(trace_.begin(), trace_.end() - 1);
    trace_.clear();
    skipCnt_ = 0;
    return {traceCopy, {}};
  }

  return {};
}
