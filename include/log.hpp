#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>

#include "log_config.hpp"
#include "log_appender_interface.hpp"
#include "util/mutex.hpp"
#include "util/singleton.hpp"

namespace plog {

static LogConfig k_log_config;

class Logger {
 public:
  static Logger *GetLogger() { return Singleton<Logger>::GetInstance(); }

  static void SetGlobalConfig(const LogConfig &log_config) {
    k_log_config = log_config;
  }

  template <class F, class... Args>
  void RegisterHandle(F &&f, Args &&...args) {
    using RetType = decltype(f(args...));
    auto task = std::make_shared<RetType()>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    functors_.emplace([task]() { (*task)(); } );
  }

  void Info(const char *format, ...);
  void Debug(const char *format, ...);
  void Warn(const char *format, ...);
  void Error(const char *format, ...);
  void Fatal(const char *format, ...);

  // 需要加锁的
  void AddAppender(const std::string &appender_name, LogAppenderInterface::Ptr appender);
  void DelAppender(const std::string &appender_name);
  void ClearAppender();

 private:

  void WriteLog(LogLevel log_level, const char *file_name, const char *function_name,
                int32_t line_num, const char *fmt, va_list ap);

  using Task = std::function<void()>;
  Mutex mutex_;
  std::map<std::string, LogAppenderInterface::Ptr> appenders_;
  std::list<Task> functors_;
};

}  // namespace plog

