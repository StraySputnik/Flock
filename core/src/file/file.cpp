#include "file/file.hpp"

#include <cstring>

namespace Flock::File {
  static constexpr const char *mode_to_str(const ModeFlag mode, const bool binary = false) {
    if (binary) {
      switch (mode) {
        case ModeFlag::Read:
          return "rb";
        case ModeFlag::Write:
          return "wb";
        case ModeFlag::ReadWrite:
          return "rb+";
        case ModeFlag::Append:
          return "ab";
        case ModeFlag::ReadAppend:
          return "ab+";
        default:
          ASSERT(false, "");
      }
    } else {
      switch (mode) {
        case ModeFlag::Read:
          return "r";
        case ModeFlag::Write:
          return "w";
        case ModeFlag::ReadWrite:
          return "r+";
        case ModeFlag::Append:
          return "a";
        case ModeFlag::ReadAppend:
          return "a+";
        default:
          ASSERT(false, "");
      }
    }

    return "";
  }

  Maybe<File> File::open_text(const Filepath &filepath, const ModeFlag mode) {
    File file{};
    file.file_ = fopen(filepath.to_string().push('\0').first(), mode_to_str(mode));
    if (!file.file_) {
      fclose(file.file_);
      return {};
    }

    return file;
  }

  Maybe<File> File::open_binary(const Filepath &filepath, const ModeFlag mode) {
    File file{};
    file.file_ = fopen(filepath.to_string().push('\0').first(), mode_to_str(mode, true));
    if (!file.file_) {
      fclose(file.file_);
      return {};
    }

    return file;
  }

  File::File(File &&other) noexcept {
    file_       = other.file_;
    other.file_ = nullptr;
  }

  File &File::operator=(File &&other) noexcept {
    if (this != &other) {
      return *this;
    }

    release();

    file_       = other.file_;
    other.file_ = nullptr;

    return *this;
  }

  File::~File() {
    release();
  }

  void File::release() {
    if (!file_) {
      return;
    }

    close();
    file_ = nullptr;
  }

  void File::close() {
    fclose(file_);
  }

  usize File::size() const {
    usize size = 0;
    fseek(file_, 0, SEEK_END);
    size = ftell(file_);
    rewind(file_);
    return size;
  }

  void File::write(const StringSlice slice) {
    fwrite(slice.first(), slice.len(), 1, file_);
  }

  void File::write(const void *ptr, const usize size) {
    fwrite(ptr, size, 1, file_);
  }

  void File::read(void *ptr, const usize size) const {
    fread(ptr, size, 1, file_);
  }

  String File::to_string() const {
    String      str{};
    const usize file_size = size();
    str.resize(file_size);
    fread(str.first(), file_size, 1, file_);
    return str;
  }

  Maybe<Directory> Directory::read_current() {
    return read(Filepath::from_c_str("."));
  }

#ifdef __unix__
#include <dirent.h>
#include <sys/stat.h>

  Maybe<Directory> Directory::read(const Filepath &filepath) {
    Directory directory{};

    String str = filepath.to_string().push('\0');

    DIR *dir = opendir(str.first());
    if (!dir) {
      return {};
    }

    dirent *dir_ent;
    while ((dir_ent = readdir(dir)) != nullptr) {
      if (!strcmp(dir_ent->d_name, ".") || !strcmp(dir_ent->d_name, "..")) {
        continue;
      }

      Filepath path = Filepath::from_c_str(str.first()).join(Filepath::from_c_str(dir_ent->d_name));
      byte     type = dir_ent->d_type;
      if (type == DT_UNKNOWN) {
        struct stat st{};
        if (lstat(dir_ent->d_name, &st) == 0) {
          type = S_ISDIR(st.st_mode) ? DT_DIR : S_ISLNK(st.st_mode) ? DT_LNK : DT_REG;
        }
      }

      if (type == DT_DIR) {
        directory.directories_.push(path);
      } else {
        directory.files_.push(path);
      }
    }

    closedir(dir);
    return directory;
  }
#elifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstring>

  Maybe<Directory> Directory::read(const Filepath &filepath) {
    Directory directory{};

    String base = filepath.to_string().push('\0');

    String pattern = filepath.to_string();
    pattern.push('\\');
    pattern.push('*');
    pattern.push('\0');

    WIN32_FIND_DATAA find_data{};
    HANDLE           handle = FindFirstFileA(pattern.first(), &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
      return {};
    }

    do {
      const char *name = find_data.cFileName;
      if (!strcmp(name, ".") || !strcmp(name, "..")) {
        continue;
      }

      Filepath path = Filepath::from_c_str(base.first()).join(Filepath::from_c_str(name));

      const DWORD attrs      = find_data.dwFileAttributes;
      const bool  is_dir     = (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
      const bool  is_reparse = (attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

      if (is_dir && !is_reparse) {
        directory.directories_.push(path);
      } else {
        directory.files_.push(path);
      }
    } while (FindNextFileA(handle, &find_data) != 0);

    const DWORD err = GetLastError();
    FindClose(handle);

    if (err != ERROR_NO_MORE_FILES) {
      return {};
    }

    return directory;
  }
#endif

  Vec<Filepath> Directory::files() const {
    return files_;
  }

  Vec<Filepath> Directory::directories() const {
    return directories_;
  }
}
