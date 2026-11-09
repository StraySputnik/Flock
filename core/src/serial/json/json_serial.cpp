#include "serial/json/json_serial.hpp"

namespace Flock::Serial::JSON {
  // TODO: Better error-handling and error logs

  Serializer Serializer::create(Flock::JSON::Object &json) {
    Serializer serializer{};
    serializer.objects_.push(&json);
    serializer.is_array_vec_.push(false);
    return serializer;
  }

  bool Serializer::begin_object(const String &key) {
    if (*is_array_vec_.last()) {
      (*arrays_.last())->push(Flock::JSON::Value::create_object());
      objects_.push((*arrays_.last())->last()->as_object());
    } else {
      (**objects_.last())[key] = Flock::JSON::Value::create_object();
      objects_.push((**objects_.last())[key].as_object());
    }

    is_array_vec_.push(false);
    return true;
  }

  bool Serializer::begin_array(const String &key, usize &len) {
    if (*is_array_vec_.last()) {
      (*arrays_.last())->push(Flock::JSON::Value::create_array());
      arrays_.push((*arrays_.last())->last()->as_array());
    } else {
      (**objects_.last())[key] = Flock::JSON::Value::create_array();
      arrays_.push((**objects_.last())[key].as_array());
    }

    is_array_vec_.push(true);
    return true;
  }

  bool Serializer::end_object() {
    if (!objects_.last()) {
      return false;
    }

    if (*is_array_vec_.last()) {
      return false;
    }

    objects_.pop();
    is_array_vec_.pop();
    return true;
  }

  bool Serializer::end_array() {
    if (!arrays_.last()) {
      return false;
    }

    if (!*is_array_vec_.last()) {
      return false;
    }

    arrays_.pop();
    is_array_vec_.pop();
    return true;
  }

  bool Serializer::serialize(const String &key, i32 &value) {
    return serialize_impl(key, Flock::JSON::Value::create_integer(value));
  }

  bool Serializer::serialize(const String &key, f32 &value) {
    return serialize_impl(key, Flock::JSON::Value::create_floating(value));
  }

  bool Serializer::serialize(const String &key, String &value) {
    return serialize_impl(key, Flock::JSON::Value::create_string(value));
  }

  bool Serializer::serialize(const String &key, Vector2f &value) {
    begin_object(key);
    serialize("x", value.x);
    serialize("y", value.y);
    end_object();
    return true;
  }

  bool Serializer::serialize(const String &key, Vector3f &value) {
    begin_object(key);
    serialize("x", value.x);
    serialize("y", value.y);
    serialize("z", value.z);
    end_object();
    return true;
  }

  bool Serializer::serialize(const String &key, Vector4f &value) {
    begin_object(key);
    serialize("x", value.x);
    serialize("y", value.y);
    serialize("z", value.z);
    serialize("w", value.w);
    end_object();
    return true;
  }

  bool Serializer::serialize(const String &key, Color3u8 &value) {
    begin_object(key);
    serialize("r", *(i32 *) &value.r);
    serialize("g", *(i32 *) &value.g);
    serialize("b", *(i32 *) &value.b);
    end_object();
    return true;
  }

  bool Serializer::serialize(const String &key, Color4u8 &value) {
    begin_object(key);
    serialize("r", *(i32 *) &value.r);
    serialize("g", *(i32 *) &value.g);
    serialize("b", *(i32 *) &value.b);
    serialize("a", *(i32 *) &value.a);
    end_object();
    return true;
  }

  bool Serializer::serialize_impl(const String &key, const Flock::JSON::Value &value) {
    if (*is_array_vec_.last()) {
      Flock::JSON::Array &array = **arrays_.last();
      array.push(value);
    } else {
      Flock::JSON::Object &obj = **objects_.last();
      obj[key]                 = value;
    }

    return true;
  }

  Deserializer Deserializer::create(const Flock::JSON::Object &json) {
    Deserializer deserializer{};
    deserializer.root_ = json;
    deserializer.objects_.push(&deserializer.root_);
    deserializer.is_array_vec_.push(false);
    return deserializer;
  }

  bool Deserializer::begin_object(const String &key) {
    if (*is_array_vec_.last()) {
      if ((*arrays_.last())->is_empty() || !(*arrays_.last())->last()->as_object()) {
        return false;
      }

      objects_.push((*arrays_.last())->last()->as_object());
      (*arrays_.last())->pop();
    } else {
      if (!(*objects_.last())->has(key) || !(*objects_.last())->get(key)->as_object()) {
        return false;
      }

      objects_.push((*objects_.last())->get(key)->as_object());
    }

    is_array_vec_.push(false);
    return true;
  }

  bool Deserializer::begin_array(const String &key, usize &len) {
    if (*is_array_vec_.last()) {
      if ((*arrays_.last())->is_empty() || !(*arrays_.last())->last()->as_array()) {
        return false;
      }

      arrays_.push((*arrays_.last())->last()->as_array());
      (*arrays_.last())->pop();
    } else {
      if (!(*objects_.last())->has(key) || !(*objects_.last())->get(key)->as_array()) {
        return false;
      }

      arrays_.push((*objects_.last())->get(key)->as_array());
    }

    is_array_vec_.push(true);
    len = (*arrays_.last())->len();
    return true;
  }

  bool Deserializer::end_object() {
    if (!objects_.last()) {
      return false;
    }

    if (*is_array_vec_.last()) {
      return false;
    }

    objects_.pop();
    is_array_vec_.pop();
    return true;
  }

  bool Deserializer::end_array() {
    if (!arrays_.last()) {
      return false;
    }

    if (!*is_array_vec_.last()) {
      return false;
    }

    arrays_.pop();
    is_array_vec_.pop();
    return true;
  }

  bool Deserializer::serialize(const String &key, i32 &value) {
    if (*is_array_vec_.last()) {
      if ((*arrays_.last())->is_empty()) {
        return false;
      }

      value = *(*arrays_.last())->pop()->as_integer();
    } else {
      if (!(*objects_.last())->has(key)) {
        return false;
      }

      value = *(*objects_.last())->get(key)->as_integer();
    }

    return true;
  }

  bool Deserializer::serialize(const String &key, f32 &value) {
    if (*is_array_vec_.last()) {
      if ((*arrays_.last())->is_empty()) {
        return false;
      }

      value = *(*arrays_.last())->pop()->as_floating();
    } else {
      if (!(*objects_.last())->has(key)) {
        return false;
      }

      value = *(*objects_.last())->get(key)->as_floating();
    }

    return true;
  }

  bool Deserializer::serialize(const String &key, String &value) {
    if (*is_array_vec_.last()) {
      if ((*arrays_.last())->is_empty()) {
        return false;
      }

      value = *(*arrays_.last())->pop()->as_string();
    } else {
      if (!(*objects_.last())->has(key)) {
        return false;
      }

      value = *(*objects_.last())->get(key)->as_string();
    }

    return true;
  }

  bool Deserializer::serialize(const String &key, Vector2f &value) {
    begin_object(key);
    if (!serialize("x", value.x)) {
      return false;
    }

    if (!serialize("y", value.y)) {
      return false;
    }

    end_object();
    return true;
  }

  bool Deserializer::serialize(const String &key, Vector3f &value) {
    begin_object(key);
    if (!serialize("x", value.x)) {
      return false;
    }

    if (!serialize("y", value.y)) {
      return false;
    }

    if (!serialize("z", value.z)) {
      return false;
    }

    end_object();
    return true;
  }

  bool Deserializer::serialize(const String &key, Vector4f &value) {
    begin_object(key);
    if (!serialize("x", value.x)) {
      return false;
    }

    if (!serialize("y", value.y)) {
      return false;
    }

    if (!serialize("z", value.z)) {
      return false;
    }

    if (!serialize("w", value.w)) {
      return false;
    }

    end_object();
    return true;
  }

  bool Deserializer::serialize(const String &key, Color3u8 &value) {
    begin_object(key);
    if (!serialize("r", *(i32 *) &value.r)) {
      return false;
    }

    if (!serialize("g", *(i32 *) &value.g)) {
      return false;
    }

    if (!serialize("b", *(i32 *) &value.b)) {
      return false;
    }

    end_object();
    return true;
  }

  bool Deserializer::serialize(const String &key, Color4u8 &value) {
    begin_object(key);
    if (!serialize("r", *(i32 *) &value.r)) {
      return false;
    }

    if (!serialize("g", *(i32 *) &value.g)) {
      return false;
    }

    if (!serialize("b", *(i32 *) &value.b)) {
      return false;
    }

    if (!serialize("a", *(i32 *) &value.a)) {
      return false;
    }

    end_object();
    return true;
  }
}
