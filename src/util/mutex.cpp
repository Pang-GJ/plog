#include "util/mutex.hpp"
#include <pthread.h>

namespace plog {

void Mutex::Lock() { pthread_mutex_lock(&mutex_); }
void Mutex::Unlock() { pthread_mutex_unlock(&mutex_); }

MutexGuard::MutexGuard(Mutex &mutex) : mutex_(mutex) { mutex_.Lock(); }
MutexGuard::~MutexGuard() { mutex_.Unlock(); }

}  // namespace plog
