#pragma once

#include "common.hpp"
#include "json/json.hpp"
#include "serial/serial.hpp"

namespace Flock::Serial::JSON {
  class FLK_API Serializer : public Archive {
    Vec<Flock::JSON::Object *> objects_      = {};
    Vec<Flock::JSON::Array *>  arrays_       = {};
    Vec<bool>                  is_array_vec_ = {};

  public:
    static Serializer create(Flock::JSON::Object &json);

    bool begin_object(const String &key) override;
    bool begin_array(const String &key, usize &len) override;
    bool end_object() override;
    bool end_array() override;

    bool serialize(const String &key, i32 &value) override;
    bool serialize(const String &key, f32 &value) override;
    bool serialize(const String &key, String &value) override;
    bool serialize(const String &key, Vector2f &value) override;
    bool serialize(const String &key, Vector3f &value) override;
    bool serialize(const String &key, Vector4f &value) override;
    bool serialize(const String &key, Color3u8 &value) override;
    bool serialize(const String &key, Color4u8 &value) override;

  private:
    bool serialize_impl(const String &key, const Flock::JSON::Value &value);
  };

  class FLK_API Deserializer : public Archive {
    Flock::JSON::Object        root_         = {};
    Vec<Flock::JSON::Object *> objects_      = {};
    Vec<Flock::JSON::Array *>  arrays_       = {};
    Vec<bool>                  is_array_vec_ = {};

  public:
    static Deserializer create(const Flock::JSON::Object &json);

    bool begin_object(const String &key) override;
    bool begin_array(const String &key, usize &len) override;
    bool end_object() override;
    bool end_array() override;

    bool serialize(const String &key, i32 &value) override;
    bool serialize(const String &key, f32 &value) override;
    bool serialize(const String &key, String &value) override;
    bool serialize(const String &key, Vector2f &value) override;
    bool serialize(const String &key, Vector3f &value) override;
    bool serialize(const String &key, Vector4f &value) override;
    bool serialize(const String &key, Color3u8 &value) override;
    bool serialize(const String &key, Color4u8 &value) override;
  };
}
