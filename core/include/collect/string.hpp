#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "vector.hpp"

namespace Flock {
    struct String {
        Vector<char> chars = {};
    };

    String string_create();
    String string_with_cap(usize cap);
    String string_with_len(usize len);
    String string_from(const char *c_str);
    void   string_delete(String *string);

    void reserve(String *string, usize cap);
    void shrink_to(String *string, usize cap);
    void shrink_to_fit(String *string);
    void resize(String *string, usize len);

    usize      len(const String *string);
    usize      cap(const String *string);
    Allocator *allocator(const String *string);

    char *get(String *string, usize idx);
    char *first(String *string);
    char *last(String *string);

    const char *get(const String *string, usize idx);
    const char *first(const String *string);
    const char *last(const String *string);

    void push(String *string, char element);
    void append(String *string, const char *c_str);
    void pop(String *string);

    bool equal(const String *lhs, const String *rhs);
    bool nequal(const String *lhs, const String *rhs);

    Hash hash(const String *string);
}
