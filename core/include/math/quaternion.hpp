#pragma once

#include <cmath>

#include "common.hpp"
#include "math.hpp"
#include "vector.hpp"

namespace Flock::Math {
    struct FLK_API Quaternion {
        f32 x = 0, y = 0, z = 0, w = 1;

        static Quaternion axis_angle(Vector3f axis, f32 angle_degrees) {
            Quaternion quat{};
            const f32  cos_theta = cosf(angle_degrees * DEG_TO_RAD / 2.0f);
            const f32  sin_theta = sinf(angle_degrees * DEG_TO_RAD / 2.0f);

            quat.w = cos_theta;
            quat.x = sin_theta * axis.x;
            quat.y = sin_theta * axis.y;
            quat.z = sin_theta * axis.z;
            return quat;
        }

        static Quaternion euler(Vector3f euler_angles_degrees) {
            // YZX Rotation Order
            auto [x, y, z] = euler_angles_degrees;
            return euler(x, y, z);
        }

        static Quaternion euler(f32 x_degrees, f32 y_degrees, f32 z_degrees) {
            // YZX Rotation Order

            Quaternion quat{};
            const f32  ax = x_degrees * DEG_TO_RAD;
            const f32  ay = y_degrees * DEG_TO_RAD;
            const f32  az = z_degrees * DEG_TO_RAD;

            const f32 cy = cosf(ay / 2.0f);
            const f32 sy = sinf(ay / 2.0f);
            const f32 cz = cosf(az / 2.0f);
            const f32 sz = sinf(az / 2.0f);
            const f32 cx = cosf(ax / 2.0f);
            const f32 sx = sinf(ax / 2.0f);

            quat.w = cy * cz * cx - sy * sz * sx;
            quat.x = cy * cz * sx + sy * sz * cx;
            quat.y = sy * cz * cx + cy * sz * sx;
            quat.z = cy * sz * cx - sy * cz * sx;
            return quat;
        }

        Quaternion conjugate() const {
            Quaternion quat{};
            quat.x = -x;
            quat.y = -y;
            quat.z = -z;
            quat.w = w;
            return quat;
        }

        f32 magnitude() const {
            return sqrtf(sqr_magnitude());
        }

        f32 sqr_magnitude() const {
            return x * x + y * y + z * z + w * w;
        }

        Quaternion normalized() const {
            return *this / magnitude();
        }

        Quaternion &normalize() {
            *this /= magnitude();
            return *this;
        }

        Quaternion inverse() const {
            return conjugate() / sqr_magnitude();
        }

        f32 dot(const Quaternion &other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        Quaternion slerp(const Quaternion &other, f32 t) const {
            // Code adapted from: https://github.com/MartinWeigel/Quaternion/blob/master/Quaternion.c
            //
            // Copyright 2018 Martin Weigel <mail@MartinWeigel.com>
            //
            // Permission to use, copy, modify, and/or distribute this software for any
            // purpose with or without fee is hereby granted, provided that the above
            // copyright notice and this permission notice appear in all copies.
            //
            // THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
            // WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
            // MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
            // ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
            // WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
            // ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
            // OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

            Quaternion result;
            const f32  cos_half_theta = w * other.w + x * other.x + y * other.y + z * other.z;

            // if q1=q2 or qa=-q2 then theta = 0 and we can return qa
            if (fabs(cos_half_theta) >= 1.0) {
                return *this;
            }

            const f32 half_theta     = acosf(cos_half_theta);
            const f32 sin_half_theta = sqrtf(1.0f - cos_half_theta * cos_half_theta);
            // If theta = 180 degrees then result is not fully defined
            // We could rotate around any axis normal to q1 or q2
            if (fabsf(sin_half_theta) < EPSILON_32) {
                result.w = 0.5f * (w + other.w);
                result.x = 0.5f * (x + other.x);
                result.y = 0.5f * (y + other.y);
                result.z = 0.5f * (z + other.z);
            } else {
                // Default quaternion calculation
                const f32 a = sinf((1 - t) * half_theta) / sin_half_theta;
                const f32 b = sinf(t * half_theta) / sin_half_theta;

                result.w = w * a + other.w * b;
                result.x = x * a + other.x * b;
                result.y = y * a + other.y * b;
                result.z = z * a + other.z * b;
            }

            return result;
        }

        Vector3f euler_angles() const {
            // YZX Rotation Order

            Vector3f  result{};
            const f32 test = x * y + z * w;
            if (test > 0.499) {
                // singularity at north pole
                result.y = 2.0f * atan2f(x, w);
                result.z = PI / 2.0f;
                result.x = 0.0f;

                return result * RAD_TO_DEG;
            }

            if (test < -0.499) {
                // singularity at south pole
                result.y = -2.0f * atan2f(x, w);
                result.z = -PI / 2.0f;
                result.x = 0.0f;

                return result * RAD_TO_DEG;
            }

            const f32 sqx = x * x;
            const f32 sqy = y * y;
            const f32 sqz = z * z;

            result.y = atan2f(2 * y * w - 2 * x * z, 1 - 2 * sqy - 2 * sqz);
            result.z = asinf(2 * test);
            result.x = atan2f(2 * x * w - 2 * y * z, 1 - 2 * sqx - 2 * sqz);

            return result * RAD_TO_DEG;
        }

        Quaternion operator+(const Quaternion &other) const {
            Quaternion quat{};
            quat.x = x + other.x;
            quat.y = y + other.y;
            quat.z = z + other.z;
            quat.w = w + other.w;
            return quat;
        }

        Quaternion operator-(const Quaternion &other) const {
            Quaternion quat{};
            quat.x = x - other.x;
            quat.y = y - other.y;
            quat.z = z - other.z;
            quat.w = w - other.w;
            return quat;
        }

        Quaternion operator*(const Quaternion &other) const {
            Quaternion quat{};
            quat.x = w * other.x + other.w * x + y * other.z - other.y * z;
            quat.y = w * other.y + other.w * y + z * other.x - other.z * x;
            quat.z = w * other.z + other.w * z + x * other.y - other.x * y;
            quat.w = w * other.w - x * other.x - y * other.y - z * other.z;
            return quat;
        }

        Quaternion operator/(const Quaternion &other) const {
            return *this * other.inverse();
        }

        Quaternion operator*(f32 other) const {
            Quaternion quat{};
            quat.x = x * other;
            quat.y = y * other;
            quat.z = z * other;
            quat.w = w * other;
            return quat;
        }

        Quaternion operator/(f32 other) const {
            Quaternion quat{};
            quat.x = x / other;
            quat.y = y / other;
            quat.z = z / other;
            quat.w = w / other;
            return quat;
        }

        Quaternion &operator+=(const Quaternion &other) {
            *this = *this + other;
            return *this;
        }

        Quaternion &operator-=(const Quaternion &other) {
            *this = *this - other;
            return *this;
        }

        Quaternion &operator*=(const Quaternion &other) {
            *this = *this * other;
            return *this;
        }

        Quaternion &operator/=(const Quaternion &other) {
            *this = *this / other;
            return *this;
        }

        Quaternion &operator*=(f32 other) {
            *this = *this * other;
            return *this;
        }

        Quaternion &operator/=(f32 other) {
            *this = *this / other;
            return *this;
        }
    };

    template <typename T>
    Vector3<T> Vector3<T>::operator*(const Quaternion &q) const {
        Vector3   p{};
        const f32 ww = q.w * q.w;
        const f32 xx = q.x * q.x;
        const f32 yy = q.y * q.y;
        const f32 zz = q.z * q.z;
        const f32 wx = q.w * q.x;
        const f32 wy = q.w * q.y;
        const f32 wz = q.w * q.z;
        const f32 xy = q.x * q.y;
        const f32 yz = q.y * q.z;
        const f32 zx = q.x * q.z;

        p.x = ww * x + 2 * wy * z - 2 * wz * y + xx * x + 2 * xy * y + 2 * zx * z - zz * x - yy * x;
        p.y = 2 * xy * x + yy * y + 2 * yz * z + 2 * q.w * z * x - zz * y + ww * y - 2 * wx * z - xx * y;
        p.z = 2 * q.x * z * x + 2 * yz * y + zz * z - 2 * wy * x - yy * z + 2 * wx * y - xx * z + ww * z;

        return p;
    }

    template <typename T>
    Vector3<T> &Vector3<T>::operator*=(const Quaternion &quat) {
        *this = *this * quat;
        return *this;
    }
}

namespace Flock {
    using Math::Quaternion;
}
