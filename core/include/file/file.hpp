#pragma once

#include "common.hpp"
#include "maybe.hpp"
#include "path.hpp"
#include "collect/string.hpp"

namespace Flock::File {
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
    static Maybe<File> open_text(const Filepath &filepath, ModeFlag mode = ModeFlag::ReadWrite);
    static Maybe<File> open_binary(const Filepath &filepath, ModeFlag mode = ModeFlag::ReadWrite);

    File() = default;

    File(const File &) = delete;
    File(File &&other) noexcept;

    File &operator=(const File &) = delete;
    File &operator=(File &&other) noexcept;

    ~File();

    void release();
    void close();

    [[nodiscard]] usize size() const;

    void write(StringSlice slice);
    void write(const void *ptr, usize size);
    void read(void *ptr, usize size) const;

    [[nodiscard]] String to_string() const;
  };

  class FLK_API Directory {
    Vec<Filepath> files_       = {};
    Vec<Filepath> directories_ = {};

  public:
    static Maybe<Directory> read_current();
    static Maybe<Directory> read(const Filepath &filepath);

    [[nodiscard]] Vec<Filepath> files() const;
    [[nodiscard]] Vec<Filepath> directories() const;

    template<typename Func>
    void traverse_files(Func &&func, const bool recurse = false) const {
      for (const auto &file: files_) {
        func(file);
      }

      if (!recurse) {
        return;
      }

      for (const auto &dir: directories_) {
        auto directory = read(dir);
        if (!directory) {
          // TODO: Print error
          continue;
        }

        directory->traverse_files(std::forward<Func>(func), recurse);
      }
    }

    template<typename Func>
    void traverse_directories(Func &&func, const bool recurse = false) const {
      for (const auto &dir: directories_) {
        func(dir);
      }

      if (!recurse) {
        return;
      }

      for (const auto &dir: directories_) {
        auto directory = read(dir);
        if (!directory) {
          // TODO: Print error
          continue;
        }

        directory->traverse_files(std::forward<Func>(func), recurse);
      }
    }
  };
}
