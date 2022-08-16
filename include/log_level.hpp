#pragma once

#include <cstdint>
#include <string>

namespace plog {

enum class LogLevel : uint8_t {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  FATAL = 5,
};

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

}  // namespace plog
