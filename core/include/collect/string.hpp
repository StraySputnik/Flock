#pragma once

#include "common.hpp"
#include "hash.hpp"
#include "vector.hpp"

namespace Flock {
    class FLK_API String {
        Vector<char> chars_ = {};

    public:
        static String create();
        static String with_cap(usize cap);
        static String with_len(usize len);
        static String from(const char *c_str);

        void free();

        void reserve(usize cap);
        void shrink_to(usize cap);
        void shrink_to_fit();
        void resize(usize len, char fill = {});
        void fill(char fill);

        usize      len() const;
        usize      cap() const;
        Allocator *allocator() const;
        bool       is_empty() const;

        char *get(usize idx);
        char &operator[](usize idx);
        char *first();
        char *last();

        const char *get(usize idx) const;
        const char &operator[](usize idx) const;
        const char *first() const;
        const char *last() const;

        void push(char element);
        void append(const String &c_str);
        void append(const char *c_str);
        void pop();
        void swap_remove(usize idx);

        bool operator==(const String &other) const;
        bool operator!=(const String &other) const;

        char *      begin();
        char *      end();
        const char *begin() const;
        const char *end() const;
    };

    FLK_API Hash hash(const String &string);
}
