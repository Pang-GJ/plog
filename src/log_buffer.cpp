#include "log_buffer.hpp"

#include <cstring>

namespace plog {

LogBuffer::LogBuffer(size_t total) : total_(total), available_(total), cur_(0) {
  data_ = new char[total];
  SetCookie(CookieStart);
}

LogBuffer::~LogBuffer() {
  delete[] data_;
  SetCookie(CookieEnd);
}

void LogBuffer::Clear() {
  cur_ = 0;
  available_ = total_;
}

void LogBuffer::Append(const char *data, size_t len) {
  memcpy(data_ + cur_, data, len);
  cur_ += len;
  available_ -= len;
}

void LogBuffer::CookieStart() {}
void LogBuffer::CookieEnd() {}

}  // namespace plog
