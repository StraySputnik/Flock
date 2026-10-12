#include "file/path.hpp"

namespace Flock::File {
  Path Path::from_string(const String &string) {
    Path path{};
    path.string_ = string;
    path.string_.replace_all('\\', '/');
    if (!path.string_.is_empty() && *path.string_.last() == '/') {
      path.string_.pop();
    }

    return path;
  }

  Path Path::from_c_str(const char *c_str) {
    Path path{};
    path.string_ = String::from_c_str(c_str);
    path.string_.replace_all('\\', '/');
    if (!path.string_.is_empty() && *path.string_.last() == '/') {
      path.string_.pop();
    }

    return path;
  }

  String Path::filename() const {
    const usize last_slash = string_.find_last('/');
    if (last_slash == NULL_POS) {
      return String::empty();
    }

    usize last_dot = string_.find_last('.');
    if (last_dot == NULL_POS || last_dot < last_slash) {
      last_dot = string_.len();
    }

    return string_.substring(last_slash + 1, last_dot - last_slash - 1).to_string();
  }

  String Path::extension() const {
    const usize last_dot = string_.find_last('.');
    if (last_dot == NULL_POS) {
      return String::empty();
    }

    return string_.substring(last_dot + 1, string_.len() - last_dot - 1).to_string();
  }

  String Path::parent_path() const {
    const usize last_slash = string_.find_last('/');
    if (last_slash == NULL_POS) {
      return String::empty();
    }

    return string_.substring(0, last_slash).to_string();
  }

  Path &Path::join(const Path &other) {
    string_.push('/');
    string_.append(other.string_);
    return *this;
  }

  String Path::to_string() const {
    String str = string_;
    str.replace_all('/', DIRECTORY_SEPARATOR);
    if (str.is_empty()) {
      str = String::from_c_str("/");
    }

    return str;
  }
}
