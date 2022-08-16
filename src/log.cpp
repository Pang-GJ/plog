#include "log.hpp"

#include <cstring>
#include <vector>
#include <ctime>

namespace plog {

std::string GetLogLevelStr(LogLevel log_level) {
  switch (log_level) {
#define XX(name)       \
  case LogLevel::name: \
    return #name;      \
    break;

    XX(TRACE);
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX

    default:
      return "UNKNOW";
  }
  return "UNKNOW";
}


void Logger::Info(const char *format, ...) {
  if (format == nullptr) {
    return;
  }
  va_list ap;

  va_start(ap, format);
  WriteLog(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__, format, ap);
  va_end(ap);
}


void Logger::Debug(const char *format, ...) {
  if (format == nullptr) {
    return;
  }
  va_list ap;

  va_start(ap, format);
  WriteLog(LogLevel::DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ap);
  va_end(ap);
}

void Logger::Warn(const char *format, ...) {
  if (format == nullptr) {
    return;
  }
  va_list ap;

  va_start(ap, format);
  WriteLog(LogLevel::WARN, __FILE__, __FUNCTION__, __LINE__, format, ap);
  va_end(ap);
}

void Logger::Error(const char *format, ...) {
  if (format == nullptr) {
    return;
  }
  va_list ap;

  va_start(ap, format);
  WriteLog(LogLevel::ERROR, __FILE__, __FUNCTION__, __LINE__, format, ap);
  va_end(ap);
}

void Logger::Fatal(const char *format, ...) {
  if (format == nullptr) {
    return;
  }
  va_list ap;

  va_start(ap, format);
  WriteLog(LogLevel::FATAL, __FILE__, __FUNCTION__, __LINE__, format, ap);
  va_end(ap);
}

void Logger::AddAppender(const std::string &appender_name, LogAppenderInterface::Ptr appender) {
  MutexGuard guard(mutex_);
  appenders_[appender_name] = appender;
}

void Logger::DelAppender(const std::string &appender_name) {
  MutexGuard guard(mutex_);

  for (auto it = appenders_.begin(); it != appenders_.end();) {
    if (it->first == appender_name) {
      it = appenders_.erase(it);
    } else {
      ++it;
    }
  }
}

void Logger::ClearAppender() { appenders_.clear(); }


  void Logger::WriteLog(LogLevel log_level, const char *file_name, const char *function_name,
                int32_t line_num, const char *fmt, va_list ap) {

  if (log_level < k_log_config.log_level_) {
    return;
  }

  std::string str_result;
  if (fmt != nullptr) {
    size_t length = vprintf(fmt, ap) + 1; // 获取格式化字符串长度
    std::vector<char> fmt_bufs(length, '\0');
    
    int write_n = vsnprintf(&(*fmt_bufs.begin()), fmt_bufs.size(), fmt, ap);
    if (write_n > 0) {
      str_result = &(*fmt_bufs.begin());
    }
  }

  if (str_result.empty()) {
    return;
  }
  const auto &get_source_file_name = [](const char *file_name) {
    return strrchr(file_name, '/') != nullptr ? strrchr(file_name, '/') + 1 : file_name;
  };

  // 获取当前时间
  time_t timep;
  time(&timep);
  char str_time[64];
  strftime(str_time, sizeof str_time, "%Y-%m-%d %H:%M:%S", localtime(&timep));

  std::string prefix;

  prefix.append(str_time);
  prefix.append("-");
  prefix.append(GetLogLevelStr(log_level) + "-");
  prefix.append(get_source_file_name(file_name));
  prefix.append("-");
  prefix.append(function_name);
  prefix.append("-");
  prefix.append(std::to_string(line_num) + "-");
  prefix.append(str_result);

  while (!functors_.empty()) {
    Task task = std::move(functors_.front());
    functors_.pop_front();
    task();
  }

  MutexGuard guard(mutex_);
  for (const auto &appender : appenders_) {
    appender.second->Append(prefix.data(), prefix.size());
  }
}


}  // namespace plog
