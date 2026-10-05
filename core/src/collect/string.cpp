#include "collect/string.hpp"

#include <cstring>

namespace Flock {
    String String::create() {
        String string{};
        string.chars_ = Vector<char>::create();
        return string;
    }

    String String::with_cap(usize cap) {
        String string{};
        string.chars_ = Vector<char>::with_cap(cap);
        return string;
    }

    String String::with_len(usize len) {
        String string{};
        string.chars_ = Vector<char>::with_len(len, ' ');
        return string;
    }

    String String::from(const char *c_str) {
        const usize len = strlen(c_str);

        String string{};
        string.chars_ = Vector<char>::with_cap(len);

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

    void String::resize(usize len, char fill) {
        return chars_.resize(len, fill);
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

    void String::push(char element) {
        return chars_.push(element);
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

    void String::pop() {
        return chars_.pop();
    }

    void String::swap_remove(usize idx) {
        return chars_.swap_remove(idx);
    }

    bool String::operator==(const String &other) const {
        if (chars_.len() != other.chars_.len()) {
            return false;
        }

        for (usize i = 0; i < chars_.len(); i++) {
            if (chars_[i] != other[i]) {
                return false;
            }
        }

        return true;
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
