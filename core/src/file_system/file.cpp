#include "file_system/file.hpp"

#if defined(PLATFORM_WINDOWS)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace Flock::FileSystem {
#if defined(PLATFORM_WINDOWS)

  static constexpr usize MAX_CHUNK_SIZE = 1u << 30;

  Maybe<File> File::open(const Filepath &filepath, const FileMode mode) {
    DWORD access      = 0;
    DWORD disposition = 0;
    switch (mode) {
      case FileMode::ReadOnly:
        access = GENERIC_READ;
        disposition = OPEN_EXISTING;
        break;
      case FileMode::WriteOnly:
        access = GENERIC_WRITE;
        disposition = CREATE_ALWAYS;
        break;
      case FileMode::ReadWrite:
        access = GENERIC_READ | GENERIC_WRITE;
        disposition = OPEN_EXISTING;
        break;
      default:
        return {};
    }

    const HANDLE handle = CreateFileA(
      filepath.to_native_string().c_str(), access,
      FILE_SHARE_READ, nullptr, disposition,
      FILE_ATTRIBUTE_NORMAL, nullptr
    );

    if (handle == INVALID_HANDLE_VALUE) {
      return {};
    }

    File file{};
    file.handle_ = handle;
    return file;
  }

  File::File(File &&other) noexcept {
    handle_       = other.handle_;
    other.handle_ = nullptr;
  }

  File &File::operator=(File &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    release();

    handle_       = other.handle_;
    other.handle_ = nullptr;

    return *this;
  }

  File::~File() {
    release();
  }

  void File::release() {
    if (is_open()) {
      CloseHandle(handle_);
    }

    handle_ = nullptr;
  }

  bool File::is_open() const {
    return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
  }

  u64 File::size() const {
    ASSERT(is_open(), "Invalid object");

    LARGE_INTEGER s{};
    return GetFileSizeEx(handle_, &s) ? static_cast<u64>(s.QuadPart) : 0;
  }

  bool File::read_exact(void *data, const usize size, const u64 offset) const {
    ASSERT(is_open(), "Invalid object");

    auto *dst  = static_cast<u8 *>(data);
    usize done = 0;
    while (done < size) {
      const u64 pos = offset + done;

      OVERLAPPED ov{};
      ov.Offset     = static_cast<DWORD>(pos & 0xFFFFFFFFull);
      ov.OffsetHigh = static_cast<DWORD>(pos >> 32);

      const usize remaining = size - done;
      const DWORD chunk     = static_cast<DWORD>(remaining < MAX_CHUNK_SIZE ? remaining : MAX_CHUNK_SIZE);

      DWORD n = 0;
      if (!ReadFile(handle_, dst + done, chunk, &n, &ov)) {
        return false;
      }

      if (n == 0) {
        return false;
      }

      done += n;
    }

    return true;
  }

  bool File::write_all(const void *data, const usize size, const u64 offset) {
    ASSERT(is_open(), "Invalid object");

    const auto *src  = static_cast<const u8 *>(data);
    usize       done = 0;
    while (done < size) {
      const u64 pos = offset + done;

      OVERLAPPED ov{};
      ov.Offset     = static_cast<DWORD>(pos & 0xFFFFFFFFull);
      ov.OffsetHigh = static_cast<DWORD>(pos >> 32);

      const usize remaining = size - done;
      const DWORD chunk     = static_cast<DWORD>(remaining < MAX_CHUNK_SIZE ? remaining : MAX_CHUNK_SIZE);

      DWORD n = 0;
      if (!WriteFile(handle_, src + done, chunk, &n, &ov)) {
        return false;
      }

      if (n == 0) {
        return false;
      }

      done += n;
    }

    return true;
  }

#else

  Maybe<File> File::open(const Filepath &filepath, const FileMode mode) {
    i32 flags = 0;
    switch (mode) {
      case FileMode::ReadOnly:
        flags = O_RDONLY;
        break;
      case FileMode::WriteOnly:
        flags = O_WRONLY | O_CREAT | O_TRUNC;
        break;
      case FileMode::ReadWrite:
        flags = O_RDWR;
        break;
      default: return {};
    }

    const i32 fd = ::open(filepath.to_native_string().c_str(), flags | O_CLOEXEC, 0644);
    if (fd < 0) {
      return {};
    }

    File file{};
    file.fd_ = fd;
    return file;
  }

  File::File(File &&other) noexcept {
    fd_       = other.fd_;
    other.fd_ = -1;
  }

  File &File::operator=(File &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    release();

    fd_       = other.fd_;
    other.fd_ = -1;

    return *this;
  }

  File::~File() {
    release();
  }

  void File::release() {
    if (is_open()) {
      close(fd_);
    }

    fd_ = -1;
  }

  bool File::is_open() const {
    return fd_ >= 0;
  }

  u64 File::size() const {
    ASSERT(is_open(), "Invalid object");

    struct stat st{};
    return fstat(fd_, &st) == 0 ? static_cast<u64>(st.st_size) : 0;
  }

  bool File::read_exact(void *data, const usize size, const u64 offset) const {
    ASSERT(is_open(), "Invalid object");

    auto *dst  = static_cast<u8 *>(data);
    usize done = 0;
    while (done < size) {
      const isize n = pread(fd_, dst + done, size - done, static_cast<off_t>(offset + done));
      if (n < 0) {
        if (errno == EINTR) {
          continue;
        }

        return false;
      }

      if (n == 0) {
        return false;
      }

      done += static_cast<usize>(n);
    }

    return true;
  }

  bool File::write_all(const void *data, const usize size, const u64 offset) {
    ASSERT(is_open(), "Invalid object");

    const auto *src  = static_cast<const u8 *>(data);
    usize       done = 0;
    while (done < size) {
      const isize n = pwrite(fd_, src + done, size - done, static_cast<off_t>(offset + done));
      if (n < 0) {
        if (errno == EINTR) {
          continue;
        }

        return false;
      }

      if (n == 0) {
        return false;
      }

      done += static_cast<usize>(n);
    }

    return true;
  }

#endif

  Maybe<String> File::to_string() const {
    const usize sz = size();
    if (sz == 0) {
      return {};
    }

    String string = String::with_capacity(sz);
    string.resize(sz, '\0');

    if (!read_exact(string.ptr(), sz, 0)) {
      return {};
    }

    return string;
  }
}
