#pragma once

#include "common.hpp"

namespace Flock {
    using Hash = u32;

    inline Hash hash(const void *data, usize len) {
        const auto ptr  = (byte *)data;
        Hash       hash = 0;

        for (usize i = 0; i < len; i++) {
            hash += ptr[i];
            hash += hash << 10;
            hash ^= hash >> 6;
        }

        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;

        return hash;
    }

    inline Hash hash(const char *str, usize len) {
        return hash((void *)str, len);
    }

    template <typename T>
    Hash hash(const T &value) {
        return hash(&value, sizeof(T));
    }
}
