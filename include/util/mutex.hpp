#pragma once

#include <pthread.h>

namespace plog {

class Mutex {
 public:
  Mutex() { pthread_mutex_init(&mutex_, nullptr); }
  ~Mutex() { pthread_mutex_destroy(&mutex_); }

  void Lock();
  void Unlock();

  pthread_mutex_t *GetMutex() { return &mutex_; }

 private:
  pthread_mutex_t mutex_;
};

class MutexGuard {
 public:
  explicit MutexGuard(Mutex &mutex);
  ~MutexGuard();

 private:
  Mutex &mutex_;
};

}  // namespace plog
