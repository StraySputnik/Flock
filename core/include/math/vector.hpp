#pragma once

#include <cmath>

#include "common.hpp"
#include "matrix.hpp"

namespace Flock::Math {
    template <typename T>
    struct Vector2 {
        T x = 0, y = 0;

        static Vector2 xy(T x, T y) {
            return {.x = x, .y = y};
        }

        static Vector2 zero() {
            return {};
        }

        static Vector2 one() {
            return {.x = 1, .y = 1};
        }

        static Vector2 up() {
            return {.x = 0, .y = 1};
        }

        static Vector2 down() {
            return {.x = 0, .y = -1};
        }

        static Vector2 right() {
            return {.x = 1, .y = 0};
        }

        static Vector2 left() {
            return {.x = -1, .y = 0};
        }

        T magnitude() const {
            return sqrt(sqr_magnitude());
        }

        T sqr_magnitude() const {
            return x * y;
        }

        Vector2 &normalize() {
            auto mag = magnitude();
            x        /= mag;
            y        /= mag;
            return *this;
        }

        Vector2 normalized() const {
            auto mag = magnitude();
            return Vector2{
                .x = x / mag,
                .y = y / mag,
            };
        }

        T dot(const Vector2 &other) const {
            return x * other.x + y * other.y;
        }

        Vector2 operator+(const Vector2 &other) const {
            return Vector2{
                .x = x + other.x,
                .y = y + other.y,
            };
        }

        Vector2 operator-(const Vector2 &other) const {
            return Vector2{
                .x = x - other.x,
                .y = y - other.y,
            };
        }

        Vector2 operator*(const Vector2 &other) const {
            return Vector2{
                .x = x * other.x,
                .y = y * other.y,
            };
        }

        Vector2 operator*(T other) const {
            return Vector2{
                .x = x * other,
                .y = y * other,
            };
        }

        Vector2 operator/(const Vector2 &other) const {
            return Vector2{
                .x = x / other.x,
                .y = y / other.y,
            };
        }

        Vector2 operator/(T other) const {
            return Vector2{
                .x = x / other,
                .y = y / other,
            };
        }

        Vector2 &operator+=(const Vector2 &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2 &operator-=(const Vector2 &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2 &operator*=(const Vector2 &other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vector2 &operator*=(T other) {
            x *= other;
            y *= other;
            return *this;
        }

        Vector2 &operator/=(const Vector2 &other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        Vector2 &operator/=(T other) {
            x /= other;
            y /= other;
            return *this;
        }

        bool operator==(const Vector2 &other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2 &other) const {
            return !(*this == other);
        }
    };

    template <typename T>
    struct Vector3 {
        T x = 0, y = 0, z = 0;

        static Vector3 xyz(T x, T y, T z) {
            return {.x = x, .y = y, .z = z};
        }

        static Vector3 zero() {
            return {};
        }

        static Vector3 one() {
            return {.x = 1, .y = 1, .z = 1};
        }

        static Vector3 up() {
            return {.x = 0, .y = 1, .z = 0};
        }

        static Vector3 down() {
            return {.x = 0, .y = -1, .z = 0};
        }

        static Vector3 right() {
            return {.x = 1, .y = 0, .z = 0};
        }

        static Vector3 left() {
            return {.x = -1, .y = 0, .z = 0};
        }

        static Vector3 front() {
            return {.x = 0, .y = 0, .z = 1};
        }

        static Vector3 back() {
            return {.x = 0, .y = 0, .z = -1};
        }

        T magnitude() const {
            return sqrt(sqr_magnitude());
        }

        T sqr_magnitude() const {
            return x * y * z;
        }

        Vector3 &normalize() {
            auto mag = magnitude();
            x        /= mag;
            y        /= mag;
            z        /= mag;
            return *this;
        }

        Vector3 normalized() const {
            auto mag = magnitude();
            return Vector2{
                .x = x / mag,
                .y = y / mag,
                .z = z / mag,
            };
        }

        T dot(const Vector3 &other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        Vector3 cross(const Vector3 &other) const {
            return {
                .x = y * other.z - z * other.y,
                .y = x * other.z - z * other.x,
                .z = x * other.y - y * other.x
            };
        }

        Vector3 operator+(const Vector3 &other) const {
            return Vector3{
                .x = x + other.x,
                .y = y + other.y,
                .z = z + other.z,
            };
        }

        Vector3 operator-(const Vector3 &other) const {
            return Vector3{
                .x = x - other.x,
                .y = y - other.y,
                .z = z - other.z,
            };
        }

        Vector3 operator*(const Vector3 &other) const {
            return Vector3{
                .x = x * other.x,
                .y = y * other.y,
                .z = z * other.z,
            };
        }

        Vector3 operator*(T other) const {
            return Vector3{
                .x = x * other,
                .y = y * other,
                .z = z * other,
            };
        }

        Vector3 operator/(const Vector3 &other) const {
            return Vector3{
                .x = x / other.x,
                .y = y / other.y,
                .z = z / other.z,
            };
        }

        Vector3 operator/(T other) const {
            return Vector3{
                .x = x / other,
                .y = y / other,
                .z = z / other,
            };
        }

        Vector3 &operator+=(const Vector3 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3 &operator-=(const Vector3 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector3 &operator*=(const Vector3 &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        Vector3 &operator*=(T other) {
            x *= other;
            y *= other;
            z *= other;
            return *this;
        }

        Vector3 &operator/=(const Vector3 &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        Vector3 &operator/=(T other) {
            x /= other;
            y /= other;
            z /= other;
            return *this;
        }

        Vector3 operator*(const Matrix4<T> &mat) const {
            Vector3 vec{};
            vec.x = x * mat.get(0, 0) + y * mat.get(0, 1) + z * mat.get(0, 2) + 1 * mat.get(0, 3);
            vec.y = x * mat.get(1, 0) + y * mat.get(1, 1) + z * mat.get(1, 2) + 1 * mat.get(1, 3);
            vec.z = x * mat.get(2, 0) + y * mat.get(2, 1) + z * mat.get(2, 2) + 1 * mat.get(2, 3);
            return vec;
        }

        Vector3 &operator *=(const Matrix4<T> &mat) {
            *this = *this * mat;
            return *this;
        }

        bool operator==(const Vector3 &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3 &other) const {
            return !(*this == other);
        }
    };

    template <typename T>
    struct Vector4 {
        T x = 0, y = 0, z = 0, w = 0;

        static Vector4 xyzw(T x, T y, T z, T w) {
            return {.x = x, .y = y, .z = z, .w = w};
        }

        static Vector4 zero() {
            return {};
        }

        static Vector4 one() {
            return {.x = 1, .y = 1, .z = 1, .w = 1};
        }

        T magnitude() const {
            return sqrt(sqr_magnitude());
        }

        T sqr_magnitude() const {
            return x * y * z * w;
        }

        Vector4 &normalize() {
            auto mag = magnitude();
            x        /= mag;
            y        /= mag;
            z        /= mag;
            w        /= mag;
            return *this;
        }

        Vector4 normalized() const {
            auto mag = magnitude();
            return Vector2{
                .x = x / mag,
                .y = y / mag,
                .z = z / mag,
                .w = w / mag,
            };
        }

        T dot(const Vector4 &other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        Vector4 operator+(const Vector4 &other) const {
            return Vector4{
                .x = x + other.x,
                .y = y + other.y,
                .z = z + other.z,
                .w = w + other.w,
            };
        }

        Vector4 operator-(const Vector4 &other) const {
            return Vector4{
                .x = x - other.x,
                .y = y - other.y,
                .z = z - other.z,
                .w = w - other.w,
            };
        }

        Vector4 operator*(const Vector4 &other) const {
            return Vector4{
                .x = x * other.x,
                .y = y * other.y,
                .z = z * other.z,
                .w = w * other.w,
            };
        }

        Vector4 operator*(T other) const {
            return Vector4{
                .x = x * other,
                .y = y * other,
                .z = z * other,
                .w = w * other,
            };
        }

        Vector4 operator/(const Vector4 &other) const {
            return Vector4{
                .x = x / other.x,
                .y = y / other.y,
                .z = z / other.z,
                .w = w / other.w,
            };
        }

        Vector4 operator/(T other) const {
            return Vector4{
                .x = x / other,
                .y = y / other,
                .z = z / other,
                .w = w / other,
            };
        }

        Vector4 &operator+=(const Vector4 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Vector4 &operator-=(const Vector4 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Vector4 &operator*=(const Vector4 &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }

        Vector4 &operator*=(T other) {
            x *= other;
            y *= other;
            z *= other;
            w *= other;
            return *this;
        }

        Vector4 &operator/=(const Vector4 &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        Vector4 &operator/=(T other) {
            x /= other;
            y /= other;
            z /= other;
            w /= other;
            return *this;
        }

        Vector4 operator*(const Matrix4<T> &mat) const {
            Vector4 vec{};
            vec.x = x * mat.get(0, 0) + y * mat.get(0, 1) + z * mat.get(0, 2) + w * mat.get(0, 3);
            vec.y = x * mat.get(1, 0) + y * mat.get(1, 1) + z * mat.get(1, 2) + w * mat.get(1, 3);
            vec.z = x * mat.get(2, 0) + y * mat.get(2, 1) + z * mat.get(2, 2) + w * mat.get(2, 3);
            vec.w = x * mat.get(3, 0) + y * mat.get(3, 1) + z * mat.get(3, 2) + w * mat.get(3, 3);
            return vec;
        }

        Vector4 &operator *=(const Matrix4<T> &mat) {
            *this = *this * mat;
            return *this;
        }

        bool operator==(const Vector4 &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const Vector4 &other) const {
            return !(*this == other);
        }
    };
}

namespace Flock {
    using Vector2f = Math::Vector2<f32>;
    using Vector3f = Math::Vector3<f32>;
    using Vector4f = Math::Vector4<f32>;

    using Vector2i = Math::Vector2<i32>;
    using Vector3i = Math::Vector3<i32>;
    using Vector4i = Math::Vector4<i32>;

    using Vector2u = Math::Vector2<u32>;
    using Vector3u = Math::Vector3<u32>;
    using Vector4u = Math::Vector4<u32>;
}
