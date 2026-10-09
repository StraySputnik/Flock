#pragma once

#include <cmath>

#include "common.hpp"
#include "math.hpp"

namespace Flock::Math {
    template <typename T>
    struct Matrix4 {
        T m[4][4] = {};

        static Matrix4 zero() {
            return {};
        }

        static Matrix4 identity() {
            Matrix4 mat{};
            for (u8 i = 0; i < 4; i++) {
                mat.get(i, i) = 1;
            }
            return mat;
        }

        static Matrix4 translation(T x, T y, T z) {
            Matrix4 mat   = identity();
            mat.get(0, 3) = x;
            mat.get(1, 3) = y;
            mat.get(2, 3) = z;
            return mat;
        }

        static Matrix4 scale(T x, T y, T z) {
            Matrix4 mat   = identity();
            mat.get(0, 0) = x;
            mat.get(1, 1) = y;
            mat.get(2, 2) = z;
            return mat;
        }

        static Matrix4 rotation(T x, T y, T z) {
            return rotation_y(y) * rotation_x(x) * rotation_z(z);
        }

        static Matrix4 rotation_x(T angle_degrees) {
            Matrix4 mat   = identity();
            T       rad   = angle_degrees * DEG_TO_RAD;
            mat.get(1, 1) = cosf(rad);
            mat.get(2, 2) = cosf(rad);
            mat.get(1, 2) = sinf(rad);
            mat.get(2, 1) = -sinf(rad);
            return mat;
        }

        static Matrix4 rotation_y(T angle_degrees) {
            Matrix4 mat   = identity();
            T       rad   = angle_degrees * DEG_TO_RAD;
            mat.get(0, 0) = cosf(rad);
            mat.get(2, 2) = cosf(rad);
            mat.get(0, 2) = -sinf(rad);
            mat.get(2, 0) = sinf(rad);
            return mat;
        }

        static Matrix4 rotation_z(T angle_degrees) {
            Matrix4 mat   = identity();
            T       rad   = angle_degrees * DEG_TO_RAD;
            mat.get(0, 0) = cosf(rad);
            mat.get(1, 1) = cosf(rad);
            mat.get(0, 1) = sinf(rad);
            mat.get(1, 0) = -sinf(rad);
            return mat;
        }

        static Matrix4 from_array(T array[16]) {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.m[col][row] = array[col * 4 + row];
                }
            }

            return mat;
        }

        T &get(u8 row, u8 col) {
            ASSERT(row < 4, "Out of bounds access");
            ASSERT(col < 4, "Out of bounds access");
            return m[col][row];
        }

        const T &get(u8 row, u8 col) const {
            ASSERT(row < 4, "Out of bounds access");
            ASSERT(col < 4, "Out of bounds access");
            return m[col][row];
        }

        Matrix4 &transpose() {
            Matrix4 mat = *this;
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    get(row, col) = mat.get(col, row);
                }
            }

            return *this;
        }

        Matrix4 transposed() const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(col, row);
                }
            }

            return mat;
        }

        T determinant_of(u8 row, u8 col) const {
            T sub[3][3] = {};

            u8 i = 0, sub_i = 0;
            while (i < 4) {
                if (i == col) {
                    i++;
                    continue;
                }

                u8 j = 0, sub_j = 0;
                while (j < 4) {
                    if (j == row) {
                        j++;
                        continue;
                    }

                    sub[sub_i][sub_j] = get(j, i);

                    sub_j++;
                    j++;
                }

                sub_i++;
                i++;
            }

            T determinant = 0;
            determinant   += sub[0][0] * (sub[1][1] * sub[2][2] - sub[1][2] * sub[2][1]);
            determinant   -= sub[0][1] * (sub[1][0] * sub[2][2] - sub[1][2] * sub[2][0]);
            determinant   += sub[0][2] * (sub[1][0] * sub[2][1] - sub[1][1] * sub[2][0]);
            return determinant;
        }

        T determinant() const {
            T deter = 0;

            deter += get(0, 0) * determinant_of(0, 0);
            deter -= get(0, 1) * determinant_of(0, 1);
            deter += get(0, 2) * determinant_of(0, 2);
            deter -= get(0, 3) * determinant_of(0, 3);

            return deter;
        }

        Matrix4 adjugate() const {
            Matrix4 adj{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    adj.get(row, col) = determinant_of(row, col);
                }
            }

            for (u8 i = 0; i < 4 * 4; i++) {
                u8  row  = i % 4;
                u8  col  = i / 4;
                i32 sign = col % 2 == 0 ? 1 : -1;
                sign     *= i % 2 == 0 ? 1 : -1;

                adj.get(row, col) *= sign;
            }

            adj.transpose();
            return adj;
        }

        Matrix4 inverse() const {
            T deter = determinant();
            if (deter == 0) {
                return identity();
            }

            return adjugate() * (1 / deter);
        }

        Matrix4 operator+(const Matrix4 &other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(row, col) + other.get(row, col);
                }
            }

            return mat;
        }

        Matrix4 operator-(const Matrix4 &other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(row, col) - other.get(row, col);
                }
            }

            return mat;
        }

        Matrix4 operator*(const Matrix4 &other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    for (u8 n = 0; n < 4; n++) {
                        mat.get(row, col) += get(row, n) * other.get(n, col);
                    }
                }
            }

            return mat;
        }

        Matrix4 operator/(const Matrix4 &other) const {
            return *this * other.inverse();
        }

        Matrix4 operator+(T other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(row, col) + other;
                }
            }

            return mat;
        }

        Matrix4 operator-(T other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(row, col) - other;
                }
            }

            return mat;
        }

        Matrix4 operator*(T other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(row, col) * other;
                }
            }

            return mat;
        }

        Matrix4 operator/(T other) const {
            Matrix4 mat{};
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    mat.get(row, col) = get(row, col) / other;
                }
            }

            return mat;
        }

        Matrix4 &operator+=(const Matrix4 &other) {
            *this = *this + other;
            return *this;
        }

        Matrix4 &operator-=(const Matrix4 &other) {
            *this = *this - other;
            return *this;
        }

        Matrix4 &operator*=(const Matrix4 &other) {
            *this = *this * other;
            return *this;
        }

        Matrix4 &operator/=(const Matrix4 &other) {
            *this = *this / other;
            return *this;
        }

        Matrix4 &operator+=(T other) {
            *this = *this + other;
            return *this;
        }

        Matrix4 &operator-=(T other) {
            *this = *this - other;
            return *this;
        }

        Matrix4 &operator*=(T other) {
            *this = *this * other;
            return *this;
        }

        Matrix4 &operator/=(T other) {
            *this = *this / other;
            return *this;
        }

        bool operator==(const Matrix4 &other) const {
            for (u8 col = 0; col < 4; col++) {
                for (u8 row = 0; row < 4; row++) {
                    if (get(row, col) != other.get(row, col)) {
                        return false;
                    }
                }
            }

            return true;
        }

        bool operator!=(const Matrix4 &other) const {
            return !(*this == other);
        }
    };
}

namespace Flock {
    using Matrix4f = Math::Matrix4<f32>;
    using Matrix4i = Math::Matrix4<i32>;
    using Matrix4u = Math::Matrix4<u32>;
}
