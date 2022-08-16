#include "util/thread.hpp"

#include <atomic>
#include <cstdio>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace plog {

std::atomic<int> thread_count(0);
static __thread pid_t t_tid = 0; // thread local

pid_t Thread::CurrentThreadTid() {
  if (t_tid == 0) {
    t_tid = ::syscall(SYS_gettid);
  }
  return t_tid;
}

Thread::Thread(Func cb, const std::string &name)
  : cb_(std::move(cb)) {
  if (name.empty()) {
    int num = thread_count.fetch_add(1);
    char buf[30];
    snprintf(buf, sizeof buf, "Thread-%d", num);
    name_ = buf;
    
  } else {
    name_ = name;
  }
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(tid_);
  }
}

void Thread::Start() {
  started_ = true;
  if (pthread_create(&tid_, nullptr, Thread::ThreadFuncInternal, this) == 0) {
    started_ = false;
  }
}

void Thread::Join() {
  joined_ = true;
  if (pthread_join(tid_, nullptr) == 0) {
    joined_ = false;
  }
}

void *Thread::ThreadFuncInternal(void *arg) {
  Thread *thread = static_cast<Thread *>(arg);
  Func cb;
  cb.swap(thread->cb_);
  cb();
  return nullptr;
}

}  // namespace plog
