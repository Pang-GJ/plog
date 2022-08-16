#pragma once

#include <sys/types.h>

namespace plog {

class LogBuffer {
 public:
  explicit LogBuffer(size_t total = 1024 * 1024 * 10);
  ~LogBuffer();

  void Clear();

  void Append(const char *data, size_t len);

  const char *Data() const { return data_; }

  size_t Length() const { return cur_; }

  size_t Available() const { return available_; }

  // for used by GDB
  // const char *DebugString();

  void SetCookie(void (*cookie)()) { cookie_ = cookie; }

 private:
  char *data_;
  const size_t total_;
  size_t available_;
  size_t cur_;
  // Must be outline function for cookies
  static void CookieStart();
  static void CookieEnd();

  void (*cookie_)();  // 函数指针
};

}  // namespace plog
