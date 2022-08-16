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

}  // namespace plog
