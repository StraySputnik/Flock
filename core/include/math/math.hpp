#pragma once

#include "common.hpp"

namespace Flock::Math {
    static constexpr f32 PI         = 3.1415926535897932384;
    static constexpr f32 DEG_TO_RAD = PI / 180.0;
    static constexpr f32 RAD_TO_DEG = 180.0 / PI;
    static constexpr f32 EPSILON_32 = 1e-4;

    template <typename T>
    T clamp(T value, T min, T max) {
        if (value < min) {
            return min;
        }

        if (value > max) {
            return max;
        }

        return value;
    }

    template <typename T>
    u8 clamp_to_u8(T value) {
        return static_cast<u8>(clamp(value, 0, 255));
    }

    template <typename T>
    i8 clamp_to_i8(T value) {
        return static_cast<i8>(clamp(value, -128, 127));
    }

    template <typename T>
    u16 clamp_to_u16(T value) {
        return static_cast<u16>(clamp(value, 0, 65535));
    }

    template <typename T>
    i16 clamp_to_i16(T value) {
        return static_cast<i16>(clamp(value, -32768, 32767));
    }
}
