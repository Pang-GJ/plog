#include "util/count_down_latch.hpp"

namespace plog {

CountDownLatch::CountDownLatch(int count) 
  : count_(count), mutex_(), condition_(mutex_) { }

void CountDownLatch::Wait() {
  MutexGuard guard(mutex_);

  while (count_ > 0) {
    condition_.Wait();
  }
}

void CountDownLatch::CountDown() {
  MutexGuard guard(mutex_);

  --count_;
  if (count_ == 0) {
    condition_.NotifyOne();
  }
}

}  // namespace plog
