#include "log_file.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace plog {

class MMapFileWriter : public FileWriter {
 public:
  MMapFileWriter(const std::string &basename, uint32_t mem_size) {
    mem_size_ = mem_size;
    writen_ = 0;
    if (fd_ >= 0) {
      close(fd_);
    }

    fd_ = open(basename.c_str(), O_RDWR | O_CREAT | O_TRUNC,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd_ < 0) {
      fprintf(stderr, "open new file failed, errno=%d", errno);
    } else {
      int n = ftruncate(fd_, mem_size);
      (void)n;

      buffer_ = (char *)mmap(NULL, mem_size_, PROT_READ | PROT_WRITE, 
                             MAP_SHARED, fd_, 0);

      if (buffer_ == MAP_FAILED) {
        perror("map");
        fprintf(stderr, "mmap file failed, errno=%d", errno);
      }
    }
  }

  ~MMapFileWriter() override {
    if (fd_ >= 0) {
      close(fd_);
      fd_ = -1;
    }

    if (buffer_ != nullptr) {
      munmap(buffer_, mem_size_);
    }
  }

  void Append(const char *msg, int32_t len) override {
    if (len > mem_size_ - writen_) {
      perror("map overflow");
      fprintf(stderr, "mmap memory overflow, errno=%d", errno);
      return;
    }
    memcpy(buffer_ + writen_, msg, len);
    writen_ += len;
  }

  void Flush() override {
    if (buffer_ != MAP_FAILED) {
      msync(buffer_, mem_size_, MS_ASYNC);
    }
  }

  uint32_t WrittenBytes() const override { return writen_; }

 private:
  int fd_;
  char *buffer_;
  uint32_t mem_size_;
  uint32_t writen_;
};

class AppendFileWriter : public FileWriter {
 public:
  explicit AppendFileWriter(const std::string &filename)
      : fp_(::fopen(filename.c_str(), "ae")) {
    ::setbuffer(fp_, buffer_, sizeof buffer_);
  }

  ~AppendFileWriter() override {
    if (fp_ != nullptr) {
      ::fclose(fp_);
    }
  }

  void Append(const char *msg, int32_t len) override {
    size_t n = ::fwrite_unlocked(msg, 1, len, fp_);
    size_t remain = len - n;
    while (remain > 0) {
      size_t x = ::fwrite_unlocked(msg + n, 1, remain, fp_);

      if (x == 0) {
        int err = ferror(fp_);
        if (err) {
          fprintf(stderr, "AppendFile::Append() failed %s\n", strerror(err));
        }
        break;
      }

      n += x;
      remain -= x;
    }

    writen_ += len;
  }

  void Flush() override { ::fflush(fp_); }

  uint32_t WrittenBytes() const override { return writen_; }

 private:
  FILE *fp_;
  char buffer_[64 * 1024];
  uint32_t writen_ = 0;
};

std::string GetHostName() {
  // HOST_NAME_MAX 64
  // _POSIX_HOST_NAME_MAX 255
  char buf[256];
  if (::gethostname(buf, sizeof buf) == 0) {
    buf[sizeof buf - 1] = '\0';
    return buf;
  }
  return "unknownhost";
}

std::string GetLogFileName(const std::string &basename, time_t *now) {
  std::string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  struct tm tm;
  *now = time(nullptr);
  gmtime_r(now, &tm);  // FIXME: localtime_r ?
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;

  filename += GetHostName();

  char pidbuf[32];
  snprintf(pidbuf, sizeof pidbuf, ".%d", getpid());
  filename += pidbuf;

  filename += ".log";

  return filename;
}

LogFile::LogFile(const std::string &basename, int32_t roll_size,
                 int32_t flush_interval, int32_t check_interval,
                 FileWriterType file_writer_type)
    : basename_(basename),
      roll_size_(roll_size),
      check_freq_count_(check_interval),
      count_(0),
      start_of_period_(0),
      last_roll_(0),
      last_flush_(0) {
  time_t now = 0;
  std::string filename = GetLogFileName(basename_, &now);
  if (file_writer_type_ == FileWriterType::MMAPFILE) {
    file_ = std::make_shared<MMapFileWriter>(filename, roll_size_);
  } else {
    file_ = std::make_shared<AppendFileWriter>(filename);
  }
  file_writer_type_ = file_writer_type;
  RollFile();
}

LogFile::~LogFile() = default;

void LogFile::Append(const char *msg, int32_t len) {
  file_->Append(msg, len);
  if (file_->WrittenBytes() > roll_size_) {
    RollFile();
  } else {
    ++count_;
    // 隔多久要检查一下
    if (count_ >= check_freq_count_) {
      count_ = 0;
      time_t now = ::time(nullptr);
      time_t this_period = now / K_ROLL_PER_SECONDS * K_ROLL_PER_SECONDS;
      if (this_period != start_of_period_) {
        RollFile();
      } else if (now - last_flush_ > flush_interval_) {
        last_flush_ = now;
        file_->Flush();
      }
    }
  }
}

void LogFile::Flush() { file_->Flush(); }

bool LogFile::RollFile() {
  time_t now = 0;
  std::string filename = GetLogFileName(basename_, &now);
  time_t start = now / K_ROLL_PER_SECONDS * K_ROLL_PER_SECONDS;

  if (now > last_roll_) {
    last_roll_ = now;
    last_flush_ = now;
    start_of_period_ = start;
    if (file_writer_type_ == FileWriterType::MMAPFILE) {
      file_.reset(new MMapFileWriter(filename, roll_size_));
    } else {
      file_.reset(new AppendFileWriter(filename));
    }
  }

  return true;
}

}  // namespace plog
