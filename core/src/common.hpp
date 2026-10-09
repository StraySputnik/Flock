#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <csignal>

namespace Flock {
    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using usize = size_t;

    using f32 = float;
    using f64 = double;

    using byte = unsigned char;

    static constexpr u64   INVALID_64 = UINT64_MAX;
    static constexpr u32   INVALID_32 = UINT32_MAX;
    static constexpr usize INVALID    = (int)sizeof(void *) == 8 ? INVALID_64 : INVALID_32;

    using TypeID = usize;

    namespace Impl {
        inline static TypeID current_type_id = 0;
    }

    template <typename T>
    TypeID type_id() {
        static TypeID id = Impl::current_type_id++;
        return id;
    }
}

#ifdef _WIN32
#   if FLK_SHARED_BUILD
#       define FLK_API __declspec(dllexport)
#   else
#       define FLK_API __declspec(dllimport)
#   endif
#else
#   if FLK_SHARED_BUILD
#       define FLK_API __attribute__((visibility("default")))
#   else
#       define FLK_API
#   endif
#endif

#define PANIC() raise(SIGTRAP)

#define ASSERT(c, msg)                                                           \
    do {                                                                         \
        if (!(c)) {                                                              \
            fprintf(stderr, "Assertion Failed: (%s), Message: \"%s\"", #c, msg); \
            PANIC();                                                             \
        }                                                                        \
    } while (0)
