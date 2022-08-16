#pragma once

#include <pthread.h>
#include <cstdint>

#include "util/mutex.hpp"

namespace plog {

class Condition {
 public:
  explicit Condition(Mutex &mutex);
  ~Condition();

  void Wait();
  bool WatiForSeconds(int32_t seconds);
  void NotifyOne();
  void NotifyAll();

 private:
  Mutex &mutex_;
  pthread_cond_t cond_;
};

}  // namespace plog
