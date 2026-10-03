#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstdio>

namespace Flock {
    using u8  = int8_t;
    using u16 = int16_t;
    using u32 = int32_t;
    using u64 = int64_t;

    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using usize = size_t;

    using f32 = float;
    using f64 = double;

    using byte = unsigned char;

    template <typename T>
    using Deleter = void (*)(T *);

    static constexpr usize INVALID_64 = UINT64_MAX;
    static constexpr u32   INVALID_32 = UINT32_MAX;
    static constexpr u32   INVALID    = INVALID_32;

    template <typename T>
    bool equal(const T *lhs, const T *rhs) {
        return *lhs == *rhs;
    }

    template <typename T>
    bool nequal(const T *lhs, const T *rhs) {
        return *lhs != *rhs;
    }

    template <typename T>
    T copy(const T *value) {
        return *value;
    }

    template <typename T>
    T move(T *value) {
        return *value;
    }
}

#define PANIC() abort()

#define ASSERT(c, msg)                                                           \
    do {                                                                         \
        if (!(c)) {                                                              \
            fprintf(stderr, "Assertion Failed: (%s), Message: \"%s\"", #c, msg); \
            PANIC();                                                             \
        }                                                                        \
    } while (0)
