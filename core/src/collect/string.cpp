#include "collect/string.hpp"

#include <cstring>

namespace Flock {
  char to_lowercase(const char ch) {
    if (ch >= 'A' && ch <= 'Z') {
      return static_cast<char>(static_cast<i32>(ch) + 'a' - 'A');
    }

    return ch;
  }

  char to_uppercase(const char ch) {
    if (ch >= 'a' && ch <= 'z') {
      return static_cast<char>(static_cast<i32>(ch) + 'A' - 'z');
    }

    return ch;
  }

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

  const char &StringSlice::operator[](const usize idx) const {
    ASSERT(idx < len_, "Out of bounds access");
    return ptr_[idx];
  }

  const char *StringSlice::first() const {
    return get(0);
  }

  const char *StringSlice::last() const {
    return get(len_ - 1);
  }

  String StringSlice::to_string() const {
    auto str = String::with_capacity(len_);
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
    string.chars_ = Vec<char>::with_capacity(1);
    string.chars_.push('\0');
    return string;
  }

  String String::with_capacity(const usize capacity) {
    String string{};
    string.chars_ = Vec<char>::with_capacity(capacity + 1);
    string.chars_.push('\0');
    return string;
  }

  String String::from_c_str(const char *c_str) {
    const usize len = strlen(c_str);

    String string{};
    string.chars_ = Vec<char>::with_capacity(len + 1);

    for (usize i = 0; i < len; i++) {
      string.chars_.push(c_str[i]);
    }
    string.chars_.push('\0');

    return string;
  }

  void String::release() {
    chars_.release();
  }

  void String::reserve(const usize capacity) {
    return chars_.reserve(capacity);
  }

  void String::reserve_exact(const usize capacity) {
    return chars_.reserve_exact(capacity);
  }

  void String::shrink_to(const usize min_capacity) {
    return chars_.shrink_to(min_capacity + 1);
  }

  void String::shrink_to_fit() {
    return chars_.shrink_to_fit();
  }

  usize String::len() const {
    return chars_.len() - 1;
  }

  usize String::capacity() const {
    return chars_.capacity() - 1;
  }

  Allocator *String::allocator() const {
    return chars_.allocator();
  }

  bool String::is_empty() const {
    return len() == 0;
  }

  char *String::ptr() {
    return chars_.ptr();
  }

  const char *String::ptr() const {
    return chars_.ptr();
  }

  const char *String::c_str() const {
    return chars_.ptr();
  }

  char *String::get(const usize idx) {
    ASSERT(idx < len(), "Out of bounds access");
    return chars_.get(idx);
  }

  char &String::operator[](const usize idx) {
    ASSERT(idx < len(), "Out of bounds access");
    return chars_[idx];
  }

  char *String::first() {
    return is_empty() ? nullptr : chars_.first();
  }

  char *String::last() {
    return is_empty() ? nullptr : chars_.get(len() - 1);
  }

  const char *String::get(const usize idx) const {
    ASSERT(idx < len(), "Out of bounds access");
    return chars_.get(idx);
  }

  const char &String::operator[](const usize idx) const {
    ASSERT(idx < len(), "Out of bounds access");
    return chars_[idx];
  }

  const char *String::first() const {
    return is_empty() ? nullptr : chars_.first();
  }

  const char *String::last() const {
    return is_empty() ? nullptr : chars_.get(len() - 1);
  }

  bool String::contains(const char ch) const {\
    for (usize i = 0; i < len(); i++) {
      if (chars_[i] == ch) {
        return true;
      }
    }
    return false;
  }

  void String::push(const char element) {
    if (chars_.is_empty()) {
      chars_.push('\0');
    }

    chars_[chars_.len() - 1] = element;
    chars_.push('\0');
  }

  void String::push_str(const StringSlice str) {
    chars_.reserve(str.len());
    for (usize i = 0; i < str.len(); i++) {
      push(str[i]);
    }
  }

  void String::push_str(const char *c_str) {
    return push_str(StringSlice::create(c_str, strlen(c_str)));
  }

  void String::insert(const usize idx, const char ch) {
    ASSERT(idx <= len(), "Out of bounds access");
    chars_.insert(idx, ch);
  }

  void String::insert_str(const usize idx, const StringSlice str) {
    ASSERT(idx <= len(), "Out of bounds access");

    const usize n       = str.len();
    const usize old_len = len();

    chars_.reserve(n);
    chars_.resize(chars_.len() + n, '\0');
    for (usize i = old_len + 1; i > idx; i--) {
      chars_[i - 1 + n] = chars_[i - 1];
    }

    for (usize i = 0; i < n; i++) {
      chars_[idx + i] = str[i];
    }
  }

  void String::insert_str(const usize idx, const char *c_str) {
    return insert_str(idx, StringSlice::create(c_str, strlen(c_str)));
  }

  Maybe<char> String::pop() {
    chars_.pop();
    const Maybe<char> result = chars_.pop();
    chars_.push('\0');
    return result;
  }

  char String::remove(const usize idx) {
    ASSERT(idx < len(), "Out of bounds access");
    return chars_.remove(idx);
  }

  char String::swap_remove(const usize idx) {
    ASSERT(idx < len(), "Out of bounds access");
    chars_.swap(idx, len() - 1);
    return chars_.remove(len() - 1);
  }

  void String::grow(const usize new_len, const char value) {
    if (new_len > len()) {
      resize(new_len, value);
    }
  }

  void String::truncate(const usize new_len) {
    if (new_len >= len()) {
      return;
    }
    chars_.truncate(new_len + 1);
    chars_[new_len] = '\0';
  }

  void String::resize(const usize new_len, const char value) {
    chars_.pop();
    chars_.resize(new_len, value);
    chars_.push('\0');
  }

  void String::swap(const usize a, const usize b) {
    ASSERT(a < len() && b < len(), "Out of bounds access");
    return chars_.swap(a, b);
  }

  void String::reverse() {
    chars_.pop();
    chars_.reverse();
    chars_.push('\0');
  }

  void String::sort() {
    chars_.pop();
    chars_.sort();
    chars_.push('\0');
  }

  void String::fill(const char value) {
    chars_.pop();
    chars_.fill(value);
    chars_.push('\0');
  }

  String String::to_lowercase() const {
    String str = *this;
    return str.make_lowercase();
  }

  String String::to_uppercase() const {
    String str = *this;
    return str.make_uppercase();
  }

  String &String::make_lowercase() {
    for (usize i = 0; i < len(); i++) {
      char &ch = *get(i);
      ch       = Flock::to_lowercase(ch);
    }

    return *this;
  }

  String &String::make_uppercase() {
    for (usize i = 0; i < len(); i++) {
      char &ch = *get(i);
      ch       = Flock::to_uppercase(ch);
    }

    return *this;
  }

  String String::split_off(const usize at) {
    ASSERT(at <= len(), "Out of bounds access");
    String str{};
    str.chars_ = chars_.split_off(at);
    chars_.push('\0');
    return str;
  }

  Pair<StringSlice, StringSlice> String::split_at(const usize at) {
    ASSERT(at <= len(), "Out of bounds access");
    const StringSlice first  = StringSlice::create(chars_.ptr(), at);
    const StringSlice second = StringSlice::create(chars_.ptr() + at, len() - at);

    return {.first = first, .second = second};
  }

  Maybe<usize> String::findn(const char c, usize n) const {
    for (usize i = 0; i < len(); i++) {
      if (chars_[i] == c) {
        if (n == 0) {
          return i;
        }
        n--;
      }
    }
    return {};
  }

  Maybe<usize> String::rfindn(const char c, usize n) const {
    for (usize i = len(); i > 0; i--) {
      if (chars_[i - 1] == c) {
        if (n == 0) {
          return i - 1;
        }
        n--;
      }
    }
    return {};
  }

  Maybe<usize> String::find(const char c) const {
    return findn(c, 0);
  }

  Maybe<usize> String::rfind(const char c) const {
    return rfindn(c, 0);
  }

  Maybe<usize> String::rfindn(const StringSlice string, usize n) const {
    if (string.len() > len()) {
      return {};
    }

    for (usize i = len() - string.len() + 1; i > 0; i--) {
      if (get(i - 1, string.len()) == string) {
        if (n == 0) {
          return i - 1;
        }
        n--;
      }
    }

    return {};
  }

  Maybe<usize> String::findn(const StringSlice string, usize n) const {
    if (string.len() > len()) {
      return {};
    }

    for (usize i = 0; i + string.len() <= len(); i++) {
      if (get(i, string.len()) == string) {
        if (n == 0) {
          return i;
        }
        n--;
      }
    }

    return {};
  }

  Maybe<usize> String::find(const StringSlice string) const {
    return findn(string, 0);
  }

  Maybe<usize> String::rfind(const StringSlice string) const {
    return rfindn(string, 0);
  }
\
  void String::replacen(const char c, const char replace, const usize n) {
    chars_.pop();
    chars_.replacen(c, replace, n);
    chars_.push('\0');
  }

  void String::rreplacen(const char c, const char replace, const usize n) {
    chars_.pop();
    chars_.rreplacen(c, replace, n);
    chars_.push('\0');
  }

  void String::replace(const char c, const char replace) {
    chars_.pop();
    chars_.replace(c, replace);
    chars_.push('\0');
  }

  void String::rreplace(const char c, const char replace) {
    chars_.pop();
    chars_.rreplace(c, replace);
    chars_.push('\0');
  }

  void String::replace_all(const char c, const char replace) {
    chars_.pop();
    chars_.replace_all(c, replace);
    chars_.push('\0');
  }

  StringSlice String::as_slice() const {
    return StringSlice::create(chars_.ptr(), len());
  }

  StringSlice String::get(const usize first, const usize length) const {
    ASSERT(first + length <= len(), "Out of bounds access");
    return StringSlice::create(chars_.ptr() + first, length);
  }

  bool String::operator==(const String &other) const {
    return chars_ == other.chars_;
  }

  bool String::operator!=(const String &other) const {
    return !(*this == other);
  }

  bool String::operator==(const char *other) const {
    const usize other_len = strlen(other);
    if (other_len != len()) {
      return false;
    }

    for (usize i = 0; i < other_len; i++) {
      if (chars_[i] != other[i]) {
        return false;
      }
    }

    return true;
  }

  bool String::operator!=(const char *other) const {
    return !(*this == other);
  }

  String String::operator+(const StringSlice &other) const {
    String string = with_capacity(len() + other.len());
    string.push_str(*this);
    string.push_str(other);
    return string;
  }

  String String::operator+(const char *other) const {
    return *this + StringSlice::from_c_str(other);
  }

  String::operator StringSlice() const {
    return as_slice();
  }

  char *String::begin() {
    return chars_.begin();
  }

  char *String::end() {
    return chars_.begin() + len();
  }

  const char *String::begin() const {
    return chars_.begin();
  }

  const char *String::end() const {
    return chars_.begin() + len();
  }

  Hash hash(const String &string) {
    return hash(string.first(), string.len());
  }
}
