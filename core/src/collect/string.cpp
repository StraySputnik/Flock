#include "collect/string.hpp"

#include <cstring>

namespace Flock {
    String string_create() {
        return {
            .chars = vector_create<char>()
        };
    }

    String string_with_cap(usize cap) {
        return {
            .chars = vector_with_cap<char>(cap)
        };
    }

    String string_with_len(usize len) {
        return {
            .chars = vector_with_len<char>(len)
        };
    }

    String string_from(const char *c_str) {
        const usize len = strlen(c_str);
        String      string{
            .chars = vector_with_cap<char>(len)
        };

        for (usize i = 0; i < len; i++) {
            push(&string.chars, c_str[i]);
        }

        return string;
    }

    void string_delete(String *string) {
        vector_delete(&string->chars);
    }

    void reserve(String *string, usize cap) {
        reserve(&string->chars, cap);
    }

    void shrink_to(String *string, usize cap) {
        shrink_to(&string->chars, cap);
    }

    void shrink_to_fit(String *string) {
        shrink_to_fit(&string->chars);
    }

    void resize(String *string, usize len) {
        resize(&string->chars, len);
    }

    usize len(const String *string) {
        return string->chars.len;
    }

    usize cap(const String *string) {
        return string->chars.cap;
    }

    Allocator *allocator(const String *string) {
        return string->chars.allocator;
    }

    char *get(String *string, usize idx) {
        return get(&string->chars, idx);
    }

    char *first(String *string) {
        return first(&string->chars);
    }

    char *last(String *string) {
        return last(&string->chars);
    }

    const char *get(const String *string, usize idx) {
        return get(&string->chars, idx);
    }

    const char *first(const String *string) {
        return first(&string->chars);
    }

    const char *last(const String *string) {
        return last(&string->chars);
    }

    void push(String *string, char element) {
        push(&string->chars, element);
    }

    void append(String *string, const char *c_str) {
        const usize len = strlen(c_str);
        reserve(string, string->chars.len + len);
        for (usize i = 0; i < len; i++) {
            push(&string->chars, c_str[i]);
        }
    }

    void pop(String *string) {
        pop(&string->chars);
    }

    void for_each(String *string, void (*func)(char *)) {
        for_each(&string->chars, func);
    }

    void for_each(const String *string, void (*func)(const char *)) {
        for_each(&string->chars, func);
    }

    void for_each(String *string, void *ctx, void (*func)(char *, void *)) {
        for_each(&string->chars, ctx, func);
    }

    void for_each(const String *string, void *ctx, void (*func)(const char *, void *)) {
        for_each(&string->chars, ctx, func);
    }

    bool equal(const String *lhs, const String *rhs) {
        if (lhs->chars.len != rhs->chars.len) {
            return false;
        }

        for (usize i = 0; i < lhs->chars.len; i++) {
            if (*get(&lhs->chars, i) != *get(&rhs->chars, i)) {
                return false;
            }
        }

        return true;
    }

    bool nequal(const String *lhs, const String *rhs) {
        return !equal(lhs, rhs);
    }

    Hash hash(const String *string) {
        return hash(string->chars.ptr, string->chars.len);
    }
}
