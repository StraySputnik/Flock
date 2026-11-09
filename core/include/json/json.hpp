#pragma once

#include <cstring>

#include "common.hpp"
#include "collect/hash_map.hpp"
#include "collect/string.hpp"
#include "collect/vec.hpp"

namespace Flock::JSON {
  enum class Type {
    Null,
    Integer,
    Floating,
    Boolean,
    String,
    Array,
    Object,
  };

  struct FLK_API Null {
  };

  class Value;

  using Array  = Vec<Value>;
  using Object = HashMap<String, Value>;

  class FLK_API Value {
    union {
      Null   null_ = {};
      i64    integer_;
      f64    floating_;
      bool   boolean_;
      String string_;
      Array  array_;
      Object object_;
    };

    Type type_ = Type::Null;

  public:
    static Value create_null();
    static Value create_integer(i64 value = 0);
    static Value create_floating(f64 value = 0.0);
    static Value create_boolean(bool value = false);
    static Value create_string(const String &value = "");
    static Value create_array(const Array &value = {});
    static Value create_object(const Object &value = {});

    Value() = default;

    Value(const Value &other);
    Value &operator=(const Value &other);

    ~Value();

    [[nodiscard]] Type type() const;
    [[nodiscard]] bool is_null() const;

    [[nodiscard]] const i64 *   as_integer() const;
    [[nodiscard]] const f64 *   as_floating() const;
    [[nodiscard]] const bool *  as_boolean() const;
    [[nodiscard]] const String *as_string() const;
    [[nodiscard]] const Array * as_array() const;
    [[nodiscard]] const Object *as_object() const;

    [[nodiscard]] i64 *   as_integer();
    [[nodiscard]] f64 *   as_floating();
    [[nodiscard]] bool *  as_boolean();
    [[nodiscard]] String *as_string();
    [[nodiscard]] Array * as_array();
    [[nodiscard]] Object *as_object();
  };

  Maybe<Value> FLK_API parse(const String &json);
  String FLK_API       to_string(const Value &value);
}
