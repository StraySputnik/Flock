#pragma once

#include "common.hpp"

namespace Flock {
    using Hash = u64;

    constexpr Hash hash_fnv1a(const void *s, usize len) {
        Hash h = 14695981039346656037ull;
        for (usize i = 0; i < len; i++) {
            h ^= static_cast<const byte *>(s)[i];
            h *= 1099511628211ull;
        }

        return h;
    }

    constexpr Hash hash(const char *str, usize len) {
        return hash_fnv1a(str, len);
    }

    template <typename T>
    constexpr Hash hash(const T &value) {
        return hash_fnv1a(&value, sizeof(T));
    }
}
