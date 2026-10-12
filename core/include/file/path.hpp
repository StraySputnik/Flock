#pragma once

#include "common.hpp"
#include "collect/string.hpp"

namespace Flock::File {
  class FLK_API Path {
    String string_ = {};

  public:
    static Path from_string(const String &string);
    static Path from_slice(const String &string);

    Path() = default;
  };
}
