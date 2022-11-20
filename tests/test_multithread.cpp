#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "plog.hpp"

void Multithread() {
  plog::LogConfig log_config;
  plog::Logger::SetGlobalConfig(log_config);

  std::thread t1([&]() {
    int i = 0;
    while (i < 10000) {
      log_info("%d hello wabywabo\n", i++);
    }
  });

  std::thread t2([&]() {
    int i = 0;
    while (i < 10000) {
      log_info("%d hello wabywabo\n", i++);
    }
  });

  t1.join();
  t2.join();
}

int main(int argc, char *argv[]) {
  Multithread();
  std::cout << "All done.." << std::endl;
  return 0;
}
