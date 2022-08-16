#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "file_writer_type.hpp"

namespace plog {

class FileWriter {
 public:
  FileWriter() = default;
  virtual ~FileWriter() = default;

  virtual void Append(const char *msg, int32_t len) = 0;

  virtual void Flush() = 0;

  virtual uint32_t WrittenBytes() const = 0;
};

class LogFile {
 public:
  LogFile(const std::string &basename, int32_t roll_size, 
          int32_t flush_interval, int32_t check_interval,
          FileWriterType file_writer_type);
  ~LogFile();

  void Append(const char *msg, int32_t len);

  void Flush();

  bool RollFile();

 private:
  std::string basename_;
  uint32_t roll_size_;
  uint32_t flush_interval_;
  // 多久进行一次检查
  uint32_t check_freq_count_;
  uint32_t count_;
  time_t start_of_period_;
  time_t last_roll_;
  time_t last_flush_;
  std::shared_ptr<FileWriter> file_;
  FileWriterType file_writer_type_;
  constexpr static int K_ROLL_PER_SECONDS = 60 * 60 * 24;
};

}  // namespace plog
