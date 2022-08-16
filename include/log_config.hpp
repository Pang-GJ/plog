#pragma once

#include <string>

#include "file_writer_type.hpp"
#include "log_level.hpp"

namespace plog {

struct LogConfig {
  uint32_t log_buffer_size_ = 4000;
  uint32_t log_buffer_num_ = 2;
  LogLevel log_level_ = LogLevel::INFO;

  struct FileOption {
    std::string file_path_;
    // 日志滚动大小和时间间隔，仅限asyncfileappender
    uint32_t log_flush_file_size_;
    uint32_t log_flush_interval_;
    FileWriterType file_writer_type_;

  } file_option_;
};

}  // namespace plog
