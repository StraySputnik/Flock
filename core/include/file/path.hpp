#pragma once

#include "common.hpp"
#include "collect/string.hpp"

namespace Flock::File {
#if _WIN32
  static constexpr char DIRECTORY_SEPARATOR = '\\';
#else
  static constexpr char DIRECTORY_SEPARATOR = '/';
#endif

  class FLK_API Path {
    String string_ = {};

  public:
    static Path from_string(const String &string);
    static Path from_c_str(const char *c_str);

    [[nodiscard]] String filename() const;
    [[nodiscard]] String extension() const;
    [[nodiscard]] String parent_path() const;

    Path &join(const Path &other);

    [[nodiscard]] String to_string() const;
  };
}

namespace Flock {
  using Filepath = File::Path;
}
