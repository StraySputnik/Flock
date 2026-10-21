#pragma once

#include "common.hpp"
#include "path.hpp"

namespace Flock::FileSystem {
  enum class FileMode : u8 {
    ReadOnly,
    WriteOnly,
    ReadWrite,
  };

  class FLK_API File {
#if defined(PLATFORM_WINDOWS)
    void *handle_ = nullptr;
#else
    i32 fd_ = -1;
#endif

  public:
    static Maybe<File> open(const Filepath &filepath, FileMode mode);

    File() = default;

    File(const File &) = delete;
    File(File &&other) noexcept;

    File &operator=(const File &) = delete;
    File &operator=(File &&other) noexcept;

    ~File();

    void release();

    [[nodiscard]] bool is_open() const;
    [[nodiscard]] u64  size() const;

    bool read_exact(void *data, usize size, u64 offset) const;
    bool write_all(const void *data, usize size, u64 offset);

    [[nodiscard]] Maybe<String> to_string() const;
  };
}

namespace Flock {
  using FileSystem::File;
  using FileSystem::FileMode;
}
