#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <csignal>

#ifdef _WIN32
static constexpr bool WINDOWS = true;
# define PLATFORM_WINDOWS 1
#else
static constexpr bool WINDOWS = false;
#endif

#ifdef __linux__
static constexpr bool LINUX = true;
# define PLATFORM_LINUX 1
# define PLATFORM_UNIX 1
#else
static constexpr bool LINUX = false;
#endif

#ifdef __APPLE__
static constexpr bool MAC_OS = true;
# define PLATFORM_MAC_OS 1
# define PLATFORM_UNIX 1
#else
static constexpr bool MAC_OS = false;
#endif

static constexpr bool UNIX = LINUX || MAC_OS;

#ifdef PLATFORM_WINDOWS
# include <BaseTsd.h>
using isize = SSIZE_T;
#elifdef PLATFORM_UNIX
using isize = ssize_t;
#endif

#ifdef __clang__
# define PLATFORM_CLANG 1
static constexpr bool CLANG = true;
#else
static constexpr bool CLANG = false;
#endif

#if defined(__GNUC__) && !defined(__clang__)
# define PLATFORM_GCC 1
static constexpr bool GCC = true;
#else
static constexpr bool GCC = false;
#endif

#ifdef _MSC_VER
# define PLATFORM_MSVC 1
static constexpr bool MSVC = true;
#else
static constexpr bool MSVC = false;
#endif

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
  static constexpr usize INVALID    = static_cast<i32>(sizeof(void *)) == 8 ? INVALID_64 : INVALID_32;

  using TypeID = u64;

  namespace Impl {
    inline static TypeID current_type_id = 0;
  }

  template<typename T>
  TypeID type_id() {
    static TypeID id = Impl::current_type_id++;
    return id;
  }
}

#ifdef PLATFORM_WINDOWS
# if FLK_SHARED_BUILD
#   define FLK_API __declspec(dllexport)
# else
#   define FLK_API __declspec(dllimport)
# endif
#else
# if FLK_SHARED_BUILD
#   define FLK_API __attribute__((visibility("default")))
# else
#   define FLK_API
# endif
#endif

#if defined(PLATFORM_MSVC)
# include <intrin.h>
# define PANIC() __debugbreak()
#elif defined(__has_builtin) && __has_builtin(__builtin_debugtrap)
# define PANIC() __builtin_debugtrap()
#else
# define PANIC() raise(SIGTRAP)
#endif

#if defined(PLATFORM_GCC) || defined(PLATFORM_CLANG)
# define UNREACHABLE() __builtin_unreachable()
#elif defined(PLATFORM_MSVC)
# define UNREACHABLE() __assume(0)
#else
# define UNREACHABLE() PANIC()
#endif

#define ASSERT(c, msg)                                                           \
    do {                                                                         \
        if (!(c)) {                                                              \
            fprintf(stderr, "Assertion Failed: (%s), Message: \"%s\"", #c, msg); \
            PANIC();                                                             \
        }                                                                        \
    } while (0)
