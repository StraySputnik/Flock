#include "file_system/directory.hpp"

#include <algorithm>
#include <utility>

#if defined(PLATFORM_WINDOWS)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace Flock::FileSystem {
  namespace {
    struct RawEntry {
      String path;
      bool   is_directory = false;
      bool   is_file      = false;
    };
  }

#if defined(PLATFORM_WINDOWS)

  static bool to_wide(const String &utf8, Vec<wchar_t> &out) {
    const i32 length = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (length <= 0) {
      return false;
    }

    out.resize(static_cast<usize>(length));
    return MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, out.ptr(), length) > 0;
  }

  static String from_wide(const wchar_t *wide) {
    String string = {};

    const i32 length = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (length <= 1) {
      return string;
    }

    Vec<char> buffer = Vec<char>::create();
    buffer.resize(static_cast<usize>(length));
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, buffer.ptr(), length, nullptr, nullptr);

    for (usize i = 0; i + 1 < static_cast<usize>(length); ++i) {
      string.push(buffer[i]);
    }

    return string;
  }

  static bool walk(const String &dir, const bool recursive, Vec<RawEntry> &out) {
    String pattern = dir;
    pattern.push('*');

    Vec<wchar_t> wide_pattern = Vec<wchar_t>::create();
    if (!to_wide(pattern, wide_pattern)) {
      return false;
    }

    WIN32_FIND_DATAW data{};
    const HANDLE     handle = FindFirstFileW(wide_pattern.ptr(), &data);
    if (handle == INVALID_HANDLE_VALUE) {
      return false;
    }

    do {
      const wchar_t *name = data.cFileName;
      if (name[0] == L'.' && (name[1] == L'\0' || (name[1] == L'.' && name[2] == L'\0'))) {
        continue;
      }

      const bool is_dir  = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
      const bool is_link = (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

      const String name_utf8 = from_wide(name);

      RawEntry entry;
      entry.path         = dir + name_utf8;
      entry.is_directory = is_dir;
      entry.is_file      = !is_dir;

      const bool descend = recursive && is_dir && !is_link;
      String     child   = {};
      if (descend) {
        child.push_str(entry.path);
      }

      out.push(std::move(entry));

      if (descend) {
        walk(child, true, out);
      }
    } while (FindNextFileW(handle, &data));

    FindClose(handle);
    return true;
  }

  bool Directory::exists(const Filepath &filepath) {
    Vec<wchar_t> wide = Vec<wchar_t>::create();
    if (!to_wide(filepath.to_native_string(), wide)) {
      return false;
    }

    const DWORD attributes = GetFileAttributesW(wide.ptr());
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
  }

  static void make_directory(const String &path) {
    Vec<wchar_t> wide = Vec<wchar_t>::create();
    if (to_wide(path, wide)) {
      CreateDirectoryW(wide.ptr(), nullptr); // "already exists" and other failures are caught by the final exists()
    }
  }

#else

  static bool walk(const String &dir, const bool recursive, Vec<RawEntry> &out) {
    DIR *stream = opendir(dir.c_str());
    if (!stream) {
      return false;
    }

    while (const dirent *item = readdir(stream)) {
      const char *name = item->d_name;
      if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))) {
        continue;
      }

      RawEntry entry;
      entry.path = dir + "/" + name;

      bool descend = false;
      if (item->d_type == DT_DIR) {
        entry.is_directory = true;
        entry.path.push(Filepath::NATIVE_SEP);
        descend            = true;
      } else if (item->d_type == DT_REG) {
        entry.is_file = true;
      } else {
        struct stat st{};
        if (lstat(entry.path.c_str(), &st) == 0) {
          const bool is_link = S_ISLNK(st.st_mode);
          if (is_link && stat(entry.path.c_str(), &st) != 0) {
            continue;
          }

          entry.is_directory = S_ISDIR(st.st_mode);
          entry.is_file      = S_ISREG(st.st_mode);
          descend            = entry.is_directory && !is_link;
        }
      }

      String child = {};
      if (descend) {
        child.push_str(entry.path);
      }

      out.push(std::move(entry));

      if (descend && recursive) {
        walk(child, true, out);
      }
    }

    closedir(stream);
    return true;
  }

  bool Directory::exists(const Filepath &filepath) {
    struct stat st{};
    return stat(filepath.to_native_string().c_str(), &st) == 0 && S_ISDIR(st.st_mode);
  }

  static void make_directory(const String &path) {
    mkdir(path.c_str(), 0755);
  }

#endif

  bool Directory::create(const Filepath &filepath) {
    const String full   = filepath.to_native_string();
    const char * chars  = full.c_str();
    String       prefix = {};

    for (usize i = 0; chars[i]; ++i) {
      if ((chars[i] == '/' || chars[i] == '\\') && i > 0) {
        make_directory(prefix);
      }

      prefix.push(chars[i]);
    }

    make_directory(prefix);
    return exists(filepath);
  }

  static Maybe<Vec<RawEntry>> collect(const Filepath &filepath, const bool recursive) {
    const String root = filepath.to_native_string();

    Vec<RawEntry> entries = Vec<RawEntry>::create();
    if (!walk(root, recursive, entries)) {
      return {};
    }

    return entries;
  }

  Maybe<Vec<DirectoryEntry>> Directory::list(const Filepath &filepath, const bool recursive) {
    auto entries = collect(filepath, recursive);
    if (!entries) {
      return {};
    }

    Vec<DirectoryEntry> result = Vec<DirectoryEntry>::with_capacity(entries->len());
    for (usize i = 0; i < entries->len(); ++i) {
      RawEntry &entry = (*entries)[i];

      DirectoryEntry out;
      out.path         = Filepath::from_string(entry.path);
      out.is_directory = entry.is_directory;
      result.push(std::move(out));
    }

    return result;
  }

  Maybe<Vec<Filepath>> Directory::list_files(const Filepath &filepath, const bool recursive) {
    auto entries = collect(filepath, recursive);
    if (!entries) {
      return {};
    }

    Vec<Filepath> result = Vec<Filepath>::create();
    for (usize i = 0; i < entries->len(); ++i) {
      RawEntry &entry = (*entries)[i];
      if (entry.is_file) {
        result.push(Filepath::from_string(entry.path));
      }
    }

    return result;
  }
}
