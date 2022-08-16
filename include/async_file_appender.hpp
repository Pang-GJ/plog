#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "log_buffer.hpp"
#include "log_appender_interface.hpp"
#include "util/mutex.hpp"
#include "util/condition.hpp"
#include "util/count_down_latch.hpp"
#include "util/thread.hpp"

namespace plog {

class AsyncFileAppender : public LogAppenderInterface {
 public:
  explicit AsyncFileAppender(const std::string &basename);
  ~AsyncFileAppender() override;

  void Append(const char *msg, size_t len) override;

  void Start();

  void Stop();

 private:
  
  void ThreadFunc();

  bool started_;
  bool running_;
  time_t persist_period_;
  std::string basename_;
  Mutex mutex_;
  Condition cond_;
  CountDownLatch countdown_latch_;
  Thread persist_thread_;
  std::unique_ptr<LogBuffer> cur_buffer_;
  std::vector<std::unique_ptr<LogBuffer>> buffers_;
};

}  // namespace plog
