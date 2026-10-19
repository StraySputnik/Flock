#include "file_system/path.hpp"

namespace Flock::FileSystem {
  Path Path::from_string(const String &path) {
    Path filepath{};
    filepath.parse(path.as_slice());
    return filepath;
  }

  Path Path::from_c_str(const char *path) {
    Path filepath{};
    filepath.parse(StringSlice::from_c_str(path));
    return filepath;
  }

  bool Path::is_empty() const {
    return root_.is_empty() && parts_.is_empty();
  }

  bool Path::has_root() const {
    return !root_.is_empty();
  }

  bool Path::is_absolute() const {
    return !root_.is_empty() && *root_.last() == '/';
  }

  bool Path::is_relative() const {
    return !is_absolute();
  }

  const String &Path::root() const {
    return root_;
  }

  const Vec<String> &Path::components() const {
    return parts_;
  }

  String Path::filename() const {
    return parts_.is_empty() ? String::empty() : *parts_.last();
  }

  String Path::extension() const {
    const String name = filename();
    if (name == "." || name == "..") {
      return String::empty();
    }

    auto pos = name.rfind('.');
    return !pos || pos.get() == 0
             ? String::empty()
             : name.get(pos.get() + 1, name.len() - pos.get() - 1).to_string();
  }

  String Path::stem() const {
    const String name = filename();
    const String ext  = extension();
    return name.get(0, name.len() - ext.len()).to_string();
  }

  Path Path::parent() const {
    Path p = *this;
    p.parts_.pop();
    return p;
  }

  Path Path::with_filename(const StringSlice name) const {
    Path p = parent();
    p.parts_.push(name.to_string());
    return p;
  }

  Path Path::with_extension(const StringSlice extension) const {
    if (parts_.is_empty()) {
      return *this;
    }

    String name = extension.to_string();
    if (!name.is_empty() && name[0] != '.') {
      name.insert(0, '.');
    }

    name.insert_str(0, stem().as_slice());
    return with_filename(name.as_slice());
  }

  Path Path::normalized() const {
    Path p = *this;
    return p.normalize();
  }

  Maybe<Path> Path::relative_to(const Path &path) const {
    if (!same_root(root_, path.root_)) {
      return {};
    }

    usize i = 0;
    while (i < parts_.len() && i < path.parts_.len() && parts_[i] == path.parts_[i]) {
      ++i;
    }

    Path r;
    for (usize j = i; j < path.parts_.len(); ++j) {
      if (path.parts_[j] == "..") {
        return {};
      }

      r.parts_.push(String::from_c_str(".."));
    }

    r.parts_.reserve(parts_.len() - i);
    for (; i < parts_.len(); i++) {
      r.parts_.push(parts_[i]);
    }

    return r;
  }

  Path &Path::normalize() {
    Vec<String> out;
    for (auto &p: parts_) {
      if (p == ".") continue;
      if (p == "..") {
        if (!out.is_empty() && *out.last() != "..") {
          out.pop();
          continue;
        }

        if (is_absolute()) {
          continue; // can't go above root }
        }
      }

      out.push(p);
    }

    parts_ = std::move(out);
    return *this;
  }

  Path &Path::join(const Path &path) {
    for (const auto &part: path.parts_) {
      parts_.push(part);
    }

    return *this;
  }

  String Path::to_string() const {
    String s = root_;
    for (usize i = 0; i < parts_.len(); i++) {
      if (i) {
        s.push('/');
      }

      s.push_str(parts_[i].as_slice());
    }

    return s;
  }

  String Path::to_native_string() const {
    String s = to_string();
    if constexpr (WINDOWS) {
      s.replace_all('/', '\\');
    }

    return s;
  }

  bool Path::operator==(const Path &other) const {
    return root_ == other.root_ && parts_ == other.parts_; // case-sensitive
  }

  bool Path::operator!=(const Path &other) const {
    return !(*this == other);
  }

  bool Path::same_root(const String &a, const String &b) {
    if constexpr (!WINDOWS) {
      return a == b;
    } else {
      if (a.len() != b.len()) {
        return false;
      }

      for (usize i = 0; i < a.len(); i++) {
        if (to_lowercase(a[i]) != to_lowercase(b[i])) {
          return false;
        }
      }

      return true;
    }
  }

  void Path::parse(const StringSlice slice) {
    // You just had to be different Windows...
    String t = slice.to_string();
    if constexpr (WINDOWS) {
      t.replace_all('\\', '/');
    }

    usize i        = 0;
    bool  is_alpha = (t[0] >= 'a' && t[0] <= 'z') || (t[0] >= 'A' && t[0] <= 'Z');
    if constexpr (WINDOWS) {
      if (t.len() >= 2 && is_alpha && t[1] == ':') {
        root_ = t.get(0, 2).to_string();
        i     = 2;
        if (i < t.len() && t[i] == '/') {
          root_.push('/');
          ++i;
        }
      } else if (t.len() >= 3 && t[0] == '/' && t[1] == '/' && t[2] != '/') {
        auto  a = t.findn('/', 2);
        usize e = t.len();
        if (a) {
          auto b = t.findn('/', a.get() + 1);
          if (b) {
            e = b.get();
          }
        }
        root_ = t.get(0, e).to_string();
        if (*root_.last() != '/') root_.push('/');
        i = e;
      }
    } else {
      if (!t.is_empty() && t[0] == '/') {
        root_ = String::from_c_str("/");
        i     = 1;
      }
    }

    usize dir = i;
    while (i < t.len()) {
      auto j = t.findn('/', dir);
      if (!j) {
        j = t.len();
      }

      if (j.get() > i) {
        if (t[i] == '/') {
          i++;
        }

        parts_.push(t.get(i, j.get() - i).to_string());
      }

      i = j.get() + 1;
      dir++;
    }
  }
}
