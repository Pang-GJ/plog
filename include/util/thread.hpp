#pragma once

#include <pthread.h>
#include <functional>
#include <string>

namespace plog {

class Thread {
 public:
  using Func = std::function<void(void)>;

  Thread(Func cb, const std::string &name);
  ~Thread();

  void Start();
  void Join();

  const std::string &GetName() const { return name_; }
  bool IsStarted() const { return started_; };

  static pid_t CurrentThreadTid();

 private:
  static void *ThreadFuncInternal(void *arg);

  bool started_ = false;
  bool joined_ = false;
  pthread_t tid_;
  std::string name_;
  Func cb_;
};

}  // namespace plog
