#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "vec.hpp"

namespace Flock {
  class String;

  class FLK_API StringSlice {
    const char *ptr_ = nullptr;
    usize       len_ = 0;

  public:
    static StringSlice create(const char *ptr, usize len);
    static StringSlice from_c_str(const char *c_str);

    [[nodiscard]] usize len() const;
    [[nodiscard]] bool  is_empty() const;

    [[nodiscard]] const char *get(usize idx) const;
    [[nodiscard]] const char &operator[](usize idx) const;
    [[nodiscard]] const char *first() const;
    [[nodiscard]] const char *last() const;

    [[nodiscard]] String to_string() const;

    bool operator==(const StringSlice &other) const;
    bool operator!=(const StringSlice &other) const;

    [[nodiscard]] const char *begin() const;
    [[nodiscard]] const char *end() const;
  };

  class FLK_API String {
    Vec<char> chars_ = {};

  public:
    static String empty();
    static String with_cap(usize cap);
    static String with_len(usize len);
    static String from_c_str(const char *c_str);

    void release();

    void    reserve(usize cap);
    void    shrink_to(usize cap);
    void    shrink_to_fit();
    String &resize(usize len, char fill = {});
    void    fill(char fill);

    [[nodiscard]] usize len() const;
    [[nodiscard]] usize cap() const;
    [[nodiscard]]
    [[nodiscard]] Allocator *allocator() const;
    [[nodiscard]] bool       is_empty() const;

    char *get(usize idx);
    char &operator[](usize idx);
    char *first();
    char *last();

    [[nodiscard]] const char *get(usize idx) const;
    [[nodiscard]] const char &operator[](usize idx) const;
    [[nodiscard]] const char *first() const;
    [[nodiscard]] const char *last() const;

    String &push(char element);
    void    append(const String &c_str);
    void    append(const char *c_str);
    char    pop();
    char    swap_remove(usize idx);

    [[nodiscard]] usize reverse_find_n(char c, usize n) const;
    [[nodiscard]] usize find_n(char c, usize n) const;
    [[nodiscard]] usize find(char c) const;
    [[nodiscard]] usize find_last(char c) const;

    [[nodiscard]] usize reverse_find_n(StringSlice string, usize n) const;
    [[nodiscard]] usize find_n(StringSlice string, usize n) const;
    [[nodiscard]] usize find(StringSlice string) const;
    [[nodiscard]] usize find_last(StringSlice string) const;

    void replace_n(char c, char replace, usize n);
    void reverse_replace_n(char c, char replace, usize n);
    void replace(char c, char replace);
    void replace_last(char c, char replace);
    void replace_all(char c, char replace);

    [[nodiscard]] StringSlice as_slice() const;
    [[nodiscard]] StringSlice substring(usize first, usize len) const;

    bool operator==(const String &other) const;
    bool operator!=(const String &other) const;

    char *                    begin();
    char *                    end();
    [[nodiscard]] const char *begin() const;
    [[nodiscard]] const char *end() const;
  };

  FLK_API Hash hash(const String &string);
}
