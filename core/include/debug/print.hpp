#pragma once

#include <cstdio>

#include "common.hpp"

namespace Flock::Debug {
  template<typename... Args>
  void print(const char *fmt, Args... args) {
    printf(fmt, args...);
  }

  template<typename... Args>
  void eprint(const char *fmt, Args... args) {
    fprintf(stderr, fmt, args...);
  }

  template<typename... Args>
  void println(const char *fmt, Args... args) {
    print(fmt, args...);
    printf("\n");
  }

  template<typename... Args>
  void eprintln(const char *fmt, Args... args) {
    eprint(fmt, args...);
    fprintf(stderr, "\n");
  }
}