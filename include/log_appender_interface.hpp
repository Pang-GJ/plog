#pragma once

#include <memory>

namespace plog {

class LogAppenderInterface {
 public:
  using Ptr = std::shared_ptr<LogAppenderInterface>;
  
  virtual ~LogAppenderInterface() = default;
  virtual void Append(const char *msg, size_t len) = 0;
};

}  // namespace plog
