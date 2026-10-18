#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "pair.hpp"
#include "vec.hpp"

namespace Flock {
  class String;

  char to_lowercase(char ch);
  char to_uppercase(char ch);

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
    static String with_capacity(usize capacity);
    static String from_c_str(const char *c_str);

    void release();

    void reserve(usize capacity);
    void reserve_exact(usize capacity);
    void shrink_to(usize min_capacity);
    void shrink_to_fit();

    [[nodiscard]] usize       len() const;
    [[nodiscard]] usize       capacity() const;
    [[nodiscard]] Allocator * allocator() const;
    [[nodiscard]] bool        is_empty() const;
    [[nodiscard]] char *      ptr();
    [[nodiscard]] const char *ptr() const;
    [[nodiscard]] const char *c_str() const;

    char *get(usize idx);
    char &operator[](usize idx);
    char *first();
    char *last();

    [[nodiscard]] const char *get(usize idx) const;
    [[nodiscard]] const char &operator[](usize idx) const;
    [[nodiscard]] const char *first() const;
    [[nodiscard]] const char *last() const;
    [[nodiscard]] bool        contains(char ch) const;

    void        push(char element);
    void        push_str(StringSlice str);
    void        push_str(const char *c_str);
    void        insert(usize idx, char ch);
    void        insert_str(usize idx, StringSlice str);
    void        insert_str(usize idx, const char *c_str);
    Maybe<char> pop();
    char        remove(usize idx);
    char        swap_remove(usize idx);
    void        grow(usize new_len, char value);
    void        truncate(usize new_len);
    void        resize(usize len, char value);
    void        swap(usize a, usize b);
    void        reverse();
    void        sort();
    void        fill(char value);

    [[nodiscard]] String to_lowercase() const;
    [[nodiscard]] String to_uppercase() const;

    String &make_lowercase();
    String &make_uppercase();

    String                         split_off(usize at);
    Pair<StringSlice, StringSlice> split_at(usize at);

    [[nodiscard]] StringSlice as_slice() const;
    [[nodiscard]] StringSlice get(usize first, usize len) const;

    [[nodiscard]] Maybe<usize> rfindn(char c, usize n) const;
    [[nodiscard]] Maybe<usize> findn(char c, usize n) const;
    [[nodiscard]] Maybe<usize> find(char c) const;
    [[nodiscard]] Maybe<usize> rfind(char c) const;

    [[nodiscard]] Maybe<usize> rfindn(StringSlice string, usize n) const;
    [[nodiscard]] Maybe<usize> findn(StringSlice string, usize n) const;
    [[nodiscard]] Maybe<usize> find(StringSlice string) const;
    [[nodiscard]] Maybe<usize> rfind(StringSlice string) const;

    void replacen(char c, char replace, usize n);
    void rreplacen(char c, char replace, usize n);
    void replace(char c, char replace);
    void rreplace(char c, char replace);
    void replace_all(char c, char replace);

    bool operator==(const String &other) const;
    bool operator!=(const String &other) const;

    bool operator==(const char *other) const;
    bool operator!=(const char *other) const;

    String operator+(const StringSlice &other) const;
    String operator+(const char *other) const;

    operator StringSlice() const;

    char *                    begin();
    char *                    end();
    [[nodiscard]] const char *begin() const;
    [[nodiscard]] const char *end() const;
  };

  FLK_API Hash hash(const String &string);
}
