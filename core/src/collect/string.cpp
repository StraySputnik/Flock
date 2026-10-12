#include "collect/string.hpp"

#include <cstring>

namespace Flock {
  StringSlice StringSlice::create(const char *ptr, const usize len) {
    StringSlice slice{};
    slice.ptr_ = ptr;
    slice.len_ = len;
    return slice;
  }

  StringSlice StringSlice::from_c_str(const char *c_str) {
    StringSlice slice{};
    slice.ptr_ = c_str;
    slice.len_ = strlen(c_str);
    return slice;
  }

  usize StringSlice::len() const {
    return len_;
  }

  bool StringSlice::is_empty() const {
    return len_ == 0;
  }

  const char *StringSlice::get(const usize idx) const {
    if (idx >= len_) {
      return nullptr;
    }

    return ptr_ + idx;
  }

  const char &StringSlice::operator[](usize idx) const {
    ASSERT(idx >= len_, "Out of bounds access");
    return ptr_[idx];
  }

  const char *StringSlice::first() const {
    return get(0);
  }

  const char *StringSlice::last() const {
    return get(len_ - 1);
  }

  String StringSlice::to_string() const {
    auto str = String::with_cap(len_);
    for (const auto c: *this) {
      str.push(c);
    }

    return str;
  }

  bool StringSlice::operator==(const StringSlice &other) const {
    if (len_ != other.len_) {
      return false;
    }

    for (usize i = 0; i < len_; i++) {
      if (ptr_[i] != other[i]) {
        return false;
      }
    }

    return true;
  }

  bool StringSlice::operator!=(const StringSlice &other) const {
    return !(*this == other);
  }

  const char *StringSlice::begin() const {
    return ptr_;
  }

  const char *StringSlice::end() const {
    return ptr_ + len_;
  }

  String String::empty() {
    String string{};
    string.chars_ = Vec<char>::empty();
    return string;
  }

  String String::with_cap(const usize cap) {
    String string{};
    string.chars_ = Vec<char>::with_cap(cap);
    return string;
  }

  String String::with_len(const usize len) {
    String string{};
    string.chars_ = Vec<char>::with_len(len, ' ');
    return string;
  }

  String String::from_c_str(const char *c_str) {
    const usize len = strlen(c_str);

    String string{};
    string.chars_ = Vec<char>::with_cap(len);

    for (usize i = 0; i < len; i++) {
      string.chars_.push(c_str[i]);
    }

    return string;
  }

  void String::release() {
    chars_.release();
  }

  void String::reserve(const usize cap) {
    return chars_.reserve(cap);
  }

  void String::shrink_to(const usize cap) {
    return chars_.shrink_to(cap);
  }

  void String::shrink_to_fit() {
    return chars_.shrink_to_fit();
  }

  String &String::resize(const usize len, const char fill) {
    chars_.resize(len, fill);
    return *this;
  }

  void String::fill(const char fill) {
    return chars_.fill(fill);
  }

  usize String::len() const {
    return chars_.len();
  }

  usize String::cap() const {
    return chars_.cap();
  }

  Allocator *String::allocator() const {
    return chars_.allocator();
  }

  bool String::is_empty() const {
    return chars_.is_empty();
  }

  char *String::get(const usize idx) {
    return chars_.get(idx);
  }

  char &String::operator[](const usize idx) {
    return chars_[idx];
  }

  char *String::first() {
    return chars_.first();
  }

  char *String::last() {
    return chars_.last();
  }

  const char *String::get(usize idx) const {
    return chars_.get(idx);
  }

  const char &String::operator[](usize idx) const {
    return chars_[idx];
  }

  const char *String::first() const {
    return chars_.first();
  }

  const char *String::last() const {
    return chars_.last();
  }

  String &String::push(char element) {
    chars_.push(element);
    return *this;
  }

  void String::append(const String &c_str) {
    return chars_.append(c_str.chars_);
  }

  void String::append(const char *c_str) {
    const usize len = strlen(c_str);
    reserve(chars_.len() + len);
    for (usize i = 0; i < len; i++) {
      chars_.push(c_str[i]);
    }
  }

  char String::pop() {
    return chars_.pop();
  }

  char String::swap_remove(usize idx) {
    return chars_.swap_remove(idx);
  }

  usize String::reverse_find_n(const char c, const usize n) const {
    return chars_.reverse_find_n(c, n);
  }

  usize String::find_n(const char c, const usize n) const {
    return chars_.find_n(c, n);
  }

  usize String::find(const char c) const {
    return chars_.find(c);
  }

  usize String::find_last(const char c) const {
    return chars_.find_last(c);
  }

  usize String::reverse_find_n(const StringSlice string, usize n) const {
    for (usize i = len() - string.len(); i != static_cast<usize>(0) - 1; i--) {
      StringSlice slice = substring(i, string.len());
      if (slice == string && n != 0) {
        n--;
      }

      if (slice == string && n == 0) {
        return i;
      }
    }

    return NULL_POS;
  }

  usize String::find_n(const StringSlice string, usize n) const {
    for (usize i = 0; i <= len() - string.len(); i++) {
      StringSlice slice = substring(i, string.len());
      if (slice == string && n != 0) {
        n--;
      }

      if (slice == string && n == 0) {
        return i;
      }
    }

    return NULL_POS;
  }

  usize String::find(const StringSlice string) const {
    return find_n(string, 0);
  }

  usize String::find_last(const StringSlice string) const {
    return reverse_find_n(string, 0);
  }

  void String::replace_n(const char c, const char replace, const usize n) {
    chars_.replace_n(c, replace, n);
  }

  void String::reverse_replace_n(const char c, const char replace, const usize n) {
    chars_.reverse_replace_n(c, replace, n);
  }

  void String::replace(const char c, const char replace) {
    chars_.replace(c, replace);
  }

  void String::replace_last(const char c, const char replace) {
    chars_.replace_last(c, replace);
  }

  void String::replace_all(const char c, const char replace) {
    chars_.replace_all(c, replace);
  }

  StringSlice String::as_slice() const {
    return StringSlice::create(chars_.first(), chars_.len());
  }

  StringSlice String::substring(const usize first, const usize len) const {
    ASSERT(first + len <= chars_.len(), "Out of bounds access");
    return StringSlice::create(chars_.first() + first, len);
  }

  bool String::operator==(const String &other) const {
    return chars_ == other.chars_;
  }

  bool String::operator!=(const String &other) const {
    return !(*this == other);
  }

  char *String::begin() {
    return chars_.begin();
  }

  char *String::end() {
    return chars_.end();
  }

  const char *String::begin() const {
    return chars_.begin();
  }

  const char *String::end() const {
    return chars_.end();
  }

  Hash hash(const String &string) {
    return hash(string.first(), string.len());
  }
}
