#pragma once

#include "common.hpp"
#include "maybe.hpp"
#include "collect/string.hpp"

namespace Flock::FileIO {
  enum class ModeFlag : u8 {\
    Read       = 0b001,
    Write      = 0b010,
    Append     = 0b100,
    ReadWrite  = 0b011,
    ReadAppend = 0b101,
  };

  class FLK_API File {
    FILE *file_ = nullptr;

  public:
    static Maybe<File> open(ModeFlag mode = ModeFlag::ReadWrite);

    File() = default;

    File(const File &) = delete;
    File(File &&)      = delete;

    File &operator=(const File &) = delete;
    File &operator=(File &&)      = delete;

    ~File();

    void release();
    void close();

    void   write_str(StringSlice slice);
    String read_str();

    void write_bin(void *ptr, usize size);
    void read_bin(void *ptr, usize size);
  };
}
