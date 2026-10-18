#pragma once

#include "common.hpp"
#include "collect/string.hpp"

namespace Flock::FileSystem {
  class FLK_API Path {
    String      root_  = {};
    Vec<String> parts_ = {};

  public:
#ifdef PLATFORM_WINDOWS
    static constexpr char NATIVE_SEP = '\\';
#else
    static constexpr char NATIVE_SEP = '/';
#endif

    static Path from_string(const String &path);
    static Path from_c_str(const char *path);

    [[nodiscard]] bool               is_empty() const;
    [[nodiscard]] bool               has_root() const;
    [[nodiscard]] bool               is_absolute() const;
    [[nodiscard]] bool               is_relative() const;
    [[nodiscard]] const String &     root() const;
    [[nodiscard]] const Vec<String> &components() const;

    [[nodiscard]] String filename() const;
    [[nodiscard]] String extension() const;
    [[nodiscard]] String stem() const;

    [[nodiscard]] Path        parent() const;
    [[nodiscard]] Path        with_filename(StringSlice name) const;
    [[nodiscard]] Path        with_extension(StringSlice extension) const;
    [[nodiscard]] Path        normalized() const;
    [[nodiscard]] Maybe<Path> relative_to(const Path &path) const;

    Path &normalize();
    Path &join(const Path &path);

    [[nodiscard]] String to_string() const;
    [[nodiscard]] String to_native_string() const;

    bool operator==(const Path &other) const;
    bool operator!=(const Path &other) const;

  private:
    static bool same_root(const String &a, const String &b);
    void        parse(StringSlice slice);
  };
}

namespace Flock {
  using Filepath = FileSystem::Path;
}
