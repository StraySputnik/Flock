#pragma once

#include "common.hpp"
#include "collect/string.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"

namespace Flock::Serial {
  class FLK_API Archive {
  public:
    virtual ~Archive() = default;

    virtual bool begin_object(const String &key) = 0;
    virtual bool begin_array(const String &key, usize &len) = 0;
    virtual bool end_object() = 0;
    virtual bool end_array() = 0;

    virtual bool serialize(const String &key, i32 &value) = 0;
    virtual bool serialize(const String &key, f32 &value) = 0;
    virtual bool serialize(const String &key, String &value) = 0;
    virtual bool serialize(const String &key, Vector2f &value) = 0;
    virtual bool serialize(const String &key, Vector3f &value) = 0;
    virtual bool serialize(const String &key, Vector4f &value) = 0;
    virtual bool serialize(const String &key, Color3u8 &value) = 0;
    virtual bool serialize(const String &key, Color4u8 &value) = 0;
  };

  template<typename T>
    requires(
      std::is_arithmetic_v<T> ||
      std::is_same_v<T, Vector2f> ||
      std::is_same_v<T, Vector3f> ||
      std::is_same_v<T, Vector4f> ||
      std::is_same_v<T, Color3u8> ||
      std::is_same_v<T, Color4u8>
    )
  bool serialize(Archive &archive, const String &key, T &value) {
    return archive.serialize(key, value);
  }
}
