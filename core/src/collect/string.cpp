#include "collect/string.hpp"

#include <cstring>

namespace Flock {
    StringSlice StringSlice::create(const char *ptr, usize len) {
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

    const char *StringSlice::get(usize idx) const {
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
        for (const auto c : *this) {
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
        return first();
    }

    const char *StringSlice::end() const {
        return last();
    }

    String String::create() {
        String string{};
        string.chars_ = Vec<char>::create();
        return string;
    }

    String String::with_cap(usize cap) {
        String string{};
        string.chars_ = Vec<char>::with_cap(cap);
        return string;
    }

    String String::with_len(usize len) {
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

    void String::free() {
        chars_.free();
    }

    void String::reserve(usize cap) {
        return chars_.reserve(cap);
    }

    void String::shrink_to(usize cap) {
        return chars_.shrink_to(cap);
    }

    void String::shrink_to_fit() {
        return chars_.shrink_to_fit();
    }

    String &String::resize(usize len, char fill) {
        chars_.resize(len, fill);
        return *this;
    }

    void String::fill(char fill) {
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

    char *String::get(usize idx) {
        return chars_.get(idx);
    }

    char &String::operator[](usize idx) {
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

    StringSlice String::as_slice() {
        return StringSlice::create(chars_.first(), chars_.len());
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
