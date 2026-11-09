#include "graphics/graphics.hpp"

namespace Flock::Graphics {
  Uniform Uniform::from_i32(const i32 value) {
    return Uniform{.i = value, .type = UniformType::I32};
  }

  Uniform Uniform::from_f32(const f32 value) {
    return Uniform{.f = value, .type = UniformType::F32};
  }

  Uniform Uniform::from_vec2f(const Vector2f value) {
    return Uniform{.v2f = value, .type = UniformType::Vec2f};
  }

  Uniform Uniform::from_vec3f(const Vector3f value) {
    return Uniform{.v3f = value, .type = UniformType::Vec3f};
  }

  Uniform Uniform::from_vec4f(const Vector4f value) {
    return Uniform{.v4f = value, .type = UniformType::Vec4f};
  }

  Maybe<i32> Uniform::get_i32() const {
    if (type == UniformType::I32) {
      return i;
    }

    return {};
  }

  Maybe<f32> Uniform::get_f32() const {
    if (type == UniformType::F32) {
      return f;
    }

    return {};
  }

  Maybe<Vector2f> Uniform::get_vec2f() const {
    if (type == UniformType::Vec2f) {
      return v2f;
    }

    return {};
  }

  Maybe<Vector3f> Uniform::get_vec3f() const {
    if (type == UniformType::Vec3f) {
      return v3f;
    }

    return {};
  }

  Maybe<Vector4f> Uniform::get_vec4f() const {
    if (type == UniformType::Vec4f) {
      return v4f;
    }

    return {};
  }
}
