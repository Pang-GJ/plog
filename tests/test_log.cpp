#include <chrono>
#include <cstdint>
#include <iostream>

#include "plog.hpp"

void Benchmark() {
  plog::LogConfig log_config;
  plog::Logger::SetGlobalConfig(log_config);
  plog::Logger::GetLogger()->AddAppender(
      "asyncfile",
      plog::LogAppenderInterface::Ptr(new plog::AsyncFileAppender("./log/")));

  const int32_t count = 1000000;
  auto start = std::chrono::system_clock::now();
  for (int32_t index = 0; index < count; ++index) {
    plog::Logger::GetLogger()->Info("%d hello world\n", index);
  }
  auto end = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << count << "次运行耗时" << duration.count() << "us" << std::endl;
}

int main(int argc, char *argv[]) {
  Benchmark();
  std::cout << "All done.." << std::endl;
  return 0;
}
