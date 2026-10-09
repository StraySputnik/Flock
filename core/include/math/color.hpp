#pragma once

#include "vector.hpp"

namespace Flock::Math {
    struct Color3u8 {
        u8 r = 0, g = 0, b = 0;

        static Color3u8 rgb(u8 r, u8 g, u8 b) {
            return {.r = r, .g = g, .b = b};
        }

        static Color3u8 black() {
            return {};
        }

        static Color3u8 white() {
            return {.r = 255, .g = 255, .b = 255};
        }

        static Color3u8 red() {
            return {.r = 255, .g = 0, .b = 0};
        }

        static Color3u8 green() {
            return {.r = 0, .g = 255, .b = 0};
        }

        static Color3u8 blue() {
            return {.r = 0, .g = 0, .b = 255};
        }

        static Color3u8 yellow() {
            return {.r = 255, .g = 255, .b = 0};
        }

        static Color3u8 cyan() {
            return {.r = 0, .g = 255, .b = 255};
        }

        static Color3u8 purple() {
            return {.r = 255, .g = 0, .b = 255};
        }

        Vector3f normalized() const {
            return Vector3f{
                .x = static_cast<f32>(r) / 255.0f,
                .y = static_cast<f32>(g) / 255.0f,
                .z = static_cast<f32>(b) / 255.0f,
            };
        }

        Color3u8 operator+(const Color3u8 &other) const {
            return Color3u8{
                .r = clamp_to_u8(r + other.r),
                .g = clamp_to_u8(g + other.g),
                .b = clamp_to_u8(b + other.b),
            };
        }

        Color3u8 operator-(const Color3u8 &other) const {
            return Color3u8{
                .r = clamp_to_u8(r - other.r),
                .g = clamp_to_u8(g - other.g),
                .b = clamp_to_u8(b - other.b),
            };
        }

        Color3u8 operator*(const Color3u8 &other) const {
            return Color3u8{
                .r = clamp_to_u8(r * other.r),
                .g = clamp_to_u8(g * other.g),
                .b = clamp_to_u8(b * other.b),
            };
        }

        Color3u8 operator/(const Color3u8 &other) const {
            return Color3u8{
                .r = clamp_to_u8(r / other.r),
                .g = clamp_to_u8(g / other.g),
                .b = clamp_to_u8(b / other.b),
            };
        }

        Color3u8 operator*(u8 other) const {
            return Color3u8{
                .r = clamp_to_u8(r * other),
                .g = clamp_to_u8(g * other),
                .b = clamp_to_u8(b * other),
            };
        }

        Color3u8 operator/(u8 other) const {
            return Color3u8{
                .r = clamp_to_u8(r / other),
                .g = clamp_to_u8(g / other),
                .b = clamp_to_u8(b / other),
            };
        }

        Color3u8 &operator+=(const Color3u8 &other) {
            r = clamp_to_u8(r + other.r);
            g = clamp_to_u8(g + other.g);
            b = clamp_to_u8(b + other.b);
            return *this;
        }

        Color3u8 &operator-=(const Color3u8 &other) {
            r = clamp_to_u8(r - other.r);
            g = clamp_to_u8(g - other.g);
            b = clamp_to_u8(b - other.b);
            return *this;
        }

        Color3u8 &operator*=(const Color3u8 &other) {
            r = clamp_to_u8(r * other.r);
            g = clamp_to_u8(g * other.g);
            b = clamp_to_u8(b * other.b);
            return *this;
        }

        Color3u8 &operator*=(u8 other) {
            r = clamp_to_u8(r * other);
            g = clamp_to_u8(g * other);
            b = clamp_to_u8(b * other);
            return *this;
        }

        Color3u8 &operator/=(const Color3u8 &other) {
            r = clamp_to_u8(r / other.r);
            g = clamp_to_u8(g / other.g);
            b = clamp_to_u8(b / other.b);
            return *this;
        }

        Color3u8 &operator/=(u8 other) {
            r = clamp_to_u8(r / other);
            g = clamp_to_u8(g / other);
            b = clamp_to_u8(b / other);
            return *this;
        }

        bool operator==(const Color3u8 &other) const {
            return r == other.r && g == other.g && b == other.b;
        }

        bool operator!=(const Color3u8 &other) const {
            return !(*this == other);
        }
    };

    struct Color4u8 {
        u8 r = 0, g = 0, b = 0, a = 255;

        static Color4u8 rgba(u8 r, u8 g, u8 b, u8 a) {
            return {.r = r, .g = g, .b = b, .a = a};
        }

        static Color4u8 black() {
            return {};
        }

        static Color4u8 white() {
            return {.r = 255, .g = 255, .b = 255, .a = 255};
        }

        static Color4u8 transparent() {
            return {.r = 0, .g = 0, .b = 0, .a = 0};
        }

        static Color4u8 red() {
            return {.r = 255, .g = 0, .b = 0, .a = 255};
        }

        static Color4u8 green() {
            return {.r = 0, .g = 255, .b = 0, .a = 255};
        }

        static Color4u8 blue() {
            return {.r = 0, .g = 0, .b = 255, .a = 255};
        }

        static Color4u8 yellow() {
            return {.r = 255, .g = 255, .b = 0, .a = 255};
        }

        static Color4u8 cyan() {
            return {.r = 0, .g = 255, .b = 255, .a = 255};
        }

        static Color4u8 purple() {
            return {.r = 255, .g = 0, .b = 255, .a = 255};
        }

        Vector4f normalized() const {
            return Vector4f{
                .x = static_cast<f32>(r) / 255.0f,
                .y = static_cast<f32>(g) / 255.0f,
                .z = static_cast<f32>(b) / 255.0f,
                .w = static_cast<f32>(a) / 255.0f,
            };
        }

        Color4u8 operator+(const Color4u8 &other) const {
            return Color4u8{
                .r = clamp_to_u8(r + other.r),
                .g = clamp_to_u8(g + other.g),
                .b = clamp_to_u8(b + other.b),
                .a = clamp_to_u8(a + other.a),
            };
        }

        Color4u8 operator-(const Color4u8 &other) const {
            return Color4u8{
                .r = clamp_to_u8(r - other.r),
                .g = clamp_to_u8(g - other.g),
                .b = clamp_to_u8(b - other.b),
                .a = clamp_to_u8(a - other.a),
            };
        }

        Color4u8 operator*(const Color4u8 &other) const {
            return Color4u8{
                .r = clamp_to_u8(r * other.r),
                .g = clamp_to_u8(g * other.g),
                .b = clamp_to_u8(b * other.b),
                .a = clamp_to_u8(a * other.a),
            };
        }

        Color4u8 operator/(const Color4u8 &other) const {
            return Color4u8{
                .r = clamp_to_u8(r / other.r),
                .g = clamp_to_u8(g / other.g),
                .b = clamp_to_u8(b / other.b),
                .a = clamp_to_u8(a / other.a),
            };
        }

        Color4u8 operator*(u8 other) const {
            return Color4u8{
                .r = clamp_to_u8(r * other),
                .g = clamp_to_u8(g * other),
                .b = clamp_to_u8(b * other),
                .a = clamp_to_u8(a * other),
            };
        }

        Color4u8 operator/(u8 other) const {
            return Color4u8{
                .r = clamp_to_u8(r / other),
                .g = clamp_to_u8(g / other),
                .b = clamp_to_u8(b / other),
                .a = clamp_to_u8(a / other),
            };
        }

        Color4u8 &operator+=(const Color4u8 &other) {
            r = clamp_to_u8(r + other.r);
            g = clamp_to_u8(g + other.g);
            b = clamp_to_u8(b + other.b);
            a = clamp_to_u8(a + other.a);
            return *this;
        }

        Color4u8 &operator-=(const Color4u8 &other) {
            r = clamp_to_u8(r - other.r);
            g = clamp_to_u8(g - other.g);
            b = clamp_to_u8(b - other.b);
            a = clamp_to_u8(a - other.a);
            return *this;
        }

        Color4u8 &operator*=(const Color4u8 &other) {
            r = clamp_to_u8(r * other.r);
            g = clamp_to_u8(g * other.g);
            b = clamp_to_u8(b * other.b);
            a = clamp_to_u8(a * other.a);
            return *this;
        }

        Color4u8 &operator*=(u8 other) {
            r = clamp_to_u8(r * other);
            g = clamp_to_u8(g * other);
            b = clamp_to_u8(b * other);
            a = clamp_to_u8(a * other);
            return *this;
        }

        Color4u8 &operator/=(const Color4u8 &other) {
            r = clamp_to_u8(r / other.r);
            g = clamp_to_u8(g / other.g);
            b = clamp_to_u8(b / other.b);
            a = clamp_to_u8(a / other.a);
            return *this;
        }

        Color4u8 &operator/=(u8 other) {
            r = clamp_to_u8(r / other);
            g = clamp_to_u8(g / other);
            b = clamp_to_u8(b / other);
            a = clamp_to_u8(a / other);
            return *this;
        }

        bool operator==(const Color4u8 &other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        bool operator!=(const Color4u8 &other) const {
            return !(*this == other);
        }
    };
}

namespace Flock {
    using Math::Color3u8;
    using Math::Color4u8;
}
