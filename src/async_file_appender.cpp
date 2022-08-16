#include "async_file_appender.hpp"
#include "log.hpp"
#include "log_file.hpp"
#include "util/count_down_latch.hpp"

#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <memory>
#include "iostream"

namespace plog {

AsyncFileAppender::AsyncFileAppender(const std::string &basename)
    : started_(false),
      running_(false),
      persist_period_(k_log_config.file_option_.log_flush_interval_),
      basename_(basename),
      cond_(mutex_),
      countdown_latch_(1),
      persist_thread_(std::bind(&AsyncFileAppender::ThreadFunc, this),
                      "AsyncLogging"),
      cur_buffer_(new LogBuffer(k_log_config.log_buffer_size_)) {
  mkdir(basename_.c_str(), 0755);
  Start();
}

AsyncFileAppender::~AsyncFileAppender() {
  if (started_) {
    Stop();
  }
}

void AsyncFileAppender::Append(const char *msg, size_t len) {
  MutexGuard guard(mutex_);

  if (cur_buffer_->Available() >= len) {
    cur_buffer_->Append(msg, len);
  } else {
    buffers_.push_back(std::move(cur_buffer_));

    cur_buffer_ = std::make_unique<LogBuffer>(k_log_config.log_buffer_size_);
    cur_buffer_->Append(msg, len);
    cond_.NotifyOne();
  }
}

void AsyncFileAppender::Start() {
  started_ = true;
  running_ = true;
  persist_thread_.Start();
  countdown_latch_.Wait();
}

void AsyncFileAppender::Stop() {
  started_ = false;
  cond_.NotifyOne();
  persist_thread_.Join();
}

void AsyncFileAppender::ThreadFunc() {
  std::unique_ptr<LogBuffer> buffer(
      new LogBuffer(k_log_config.log_buffer_size_));
  std::vector<std::unique_ptr<LogBuffer>> persist_buffers;
  persist_buffers.reserve(k_log_config.log_buffer_num_);

  LogFile log_file(basename_, k_log_config.file_option_.log_flush_file_size_,
                   k_log_config.file_option_.log_flush_interval_, 1024,
                   k_log_config.file_option_.file_writer_type_);

  countdown_latch_.CountDown();

  while (running_) {
    {
      MutexGuard guard(mutex_);
      // wake up every persist_per_seconds_ or on Buffer is full
      if (buffers_.empty()) {
        cond_.WatiForSeconds(persist_period_);
      }

      if (buffers_.empty() && cur_buffer_->Length() == 0) {
        continue;
      }

      buffers_.push_back(std::move(cur_buffer_));

      // reset cur_buffer_ and buffers_
      persist_buffers.swap(buffers_);
      cur_buffer_ = std::move(buffer);
      cur_buffer_->Clear();
    }

    // if log is too large, drop it
    if (persist_buffers.size() > k_log_config.log_buffer_size_) {
      std::cerr << "log is too large, drop some" << std::endl;
      persist_buffers.erase(persist_buffers.begin() + 1, persist_buffers.end());
    }

    // persist log
    for (const auto &buffer : persist_buffers) {
      log_file.Append(buffer->Data(), buffer->Length());
    }

    // reset buffer and persist_buffers
    buffer = std::move(persist_buffers[0]);
    buffer->Clear();
    persist_buffers.clear();

    log_file.Flush();

    if (!started_) {
      MutexGuard guard(mutex_);
      if (cur_buffer_->Length() == 0) {
        running_ = false;
      }
    }
  }
  log_file.Flush();
}

}  // namespace plog
