#pragma once

#include "common.hpp"
#include "collect/hash_map.hpp"
#include "collect/string.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"

namespace Flock::Graphics {
  struct FLK_API Mesh {
    Vec<Vector3f> vertices   = {};
    Vec<Vector3f> normals    = {};
    Vec<Vector2f> tex_coords = {};
    Vec<u32>      indices    = {};
  };

  struct FLK_API Image {
    Vec<Color4u8> pixels = {}; // Left-to-right Top-to-bottom
    u32           width  = 0;
    u32           height = 0;
  };

  enum class FaceWinding : u8 {
    CW,
    CCW,
  };

  enum class CullingMode : u8 {
    None,
    BackFace,
    FrontFace,
  };

  enum class BlendMode : u8 {
    None,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
  };

  enum class CompareMode: u8 {
    Always,
    Less,
    LEqual,
    Greater,
    GEqual,
    Equal,
    NEqual,
  };

  struct FLK_API Pipeline {
    String      shader        = {};
    FaceWinding winding       = FaceWinding::CW;
    CullingMode culling       = CullingMode::BackFace;
    BlendMode   blending      = BlendMode::SrcAlpha;
    CompareMode depth_compare = CompareMode::Less;
    bool        depth_write   = true;
  };

  enum class UniformType : u8 {
    I32,
    F32,
    Vec2f,
    Vec3f,
    Vec4f,
  };

  struct FLK_API Uniform {
    union {
      i32      i = 0;
      f32      f;
      Vector2f v2f;
      Vector3f v3f;
      Vector4f v4f;
    };

    UniformType type = UniformType::I32;

    static Uniform from_i32(i32 value);
    static Uniform from_f32(f32 value);
    static Uniform from_vec2f(Vector2f value);
    static Uniform from_vec3f(Vector3f value);
    static Uniform from_vec4f(Vector4f value);

    [[nodiscard]] Maybe<i32>      get_i32() const;
    [[nodiscard]] Maybe<f32>      get_f32() const;
    [[nodiscard]] Maybe<Vector2f> get_vec2f() const;
    [[nodiscard]] Maybe<Vector3f> get_vec3f() const;
    [[nodiscard]] Maybe<Vector4f> get_vec4f() const;
  };

  struct FLK_API Material {
    HashMap<String, Uniform> uniforms      = {};
    String                   pipeline = {};

    void set_pipeline(const StringSlice path) { pipeline = path.to_string(); }
    void reset_pipeline() { pipeline = {}; }

    void     set_uniform(const String &name, const Uniform &value) { uniforms[name] = value; }
    Uniform *get_uniform(const String &name) { return uniforms.get(name); }
    void     remove_uniform(const String &name) { uniforms.remove(name); }
  };
}

namespace Flock {
  using Graphics::Mesh;
  using Graphics::Image;
  using Graphics::Pipeline;
  using Graphics::Material;
}
