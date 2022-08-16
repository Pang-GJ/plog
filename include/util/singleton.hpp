#pragma once

#include <cstdlib>
#include <pthread.h>

namespace plog {

template <typename T>
class Singleton {
 public:

  Singleton() = delete;
  ~Singleton() = delete;

  static T *GetInstance() {
    pthread_once(&once_control, &Singleton::Init);
    return value_;
  }

  static void Destroy() {
    if (value_ != nullptr) {
      delete value_;
    }
  }

 private:

  static void Init() {
    value_ = new T();
    ::atexit(Destroy);
  }

  static T *value_;
  static pthread_once_t once_control;
};

template <typename T>
pthread_once_t Singleton<T>::once_control = PTHREAD_ONCE_INIT;

template <typename T>
T *Singleton<T>::value_ = nullptr;

}  // namespace plog
