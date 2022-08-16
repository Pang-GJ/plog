#include "util/condition.hpp"

#include <cerrno>
#include <cstdint>

namespace plog {

Condition::Condition(Mutex &mutex)
  : mutex_(mutex) {
  pthread_cond_init(&cond_, nullptr);
}

Condition::~Condition() { pthread_cond_destroy(&cond_); }

void Condition::Wait() { pthread_cond_wait(&cond_, mutex_.GetMutex()); }

bool Condition::WatiForSeconds(int32_t seconds) {
  struct timespec abstime;
  clock_gettime(CLOCK_REALTIME, &abstime);

  const int64_t k_nano_seconds_per_second = 1000000000;
  int64_t nanoseconds = static_cast<int64_t>(seconds * k_nano_seconds_per_second);

  abstime.tv_sec = static_cast<time_t>((abstime.tv_nsec + nanoseconds) / k_nano_seconds_per_second);
  abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % k_nano_seconds_per_second);

  return ETIMEDOUT == 
    pthread_cond_timedwait(&cond_, mutex_.GetMutex(), &abstime);
}

void Condition::NotifyOne() { pthread_cond_signal(&cond_); }

void Condition::NotifyAll() { pthread_cond_broadcast(&cond_); }

}  // namespace plog
