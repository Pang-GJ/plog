#pragma once

#include <cstdint>

#include "util/condition.hpp"
#include "util/mutex.hpp"

namespace plog {

class CountDownLatch {
 public:
  explicit CountDownLatch(int count);

  void Wait();
  void CountDown();

  inline int32_t GetCount() const {
    MutexGuard lock(mutex_);
    return count_;
  }

 private:
  int32_t count_ = 0;
  mutable Mutex mutex_;
  Condition condition_;
};

}  // namespace plog
