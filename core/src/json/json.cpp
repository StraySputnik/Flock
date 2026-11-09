#include "json/json.hpp"

#include "debug/log.hpp"

namespace Flock::JSON {
  // I know how bad all of this is, but here's the thing... I don't care
  // TODO: Better error logs with line and column numbers

  Value Value::create_null() {
    return {};
  }

  Value Value::create_integer(i64 value) {
    Value val{};
    val.type_    = Type::Integer;
    val.integer_ = value;
    return val;
  }

  Value Value::create_floating(f64 value) {
    Value val{};
    val.type_     = Type::Floating;
    val.floating_ = value;
    return val;
  }

  Value Value::create_boolean(bool value) {
    Value val{};
    val.type_    = Type::Boolean;
    val.boolean_ = value;
    return val;
  }

  Value Value::create_string(const String &value) {
    Value val{};
    val.type_   = Type::String;
    val.string_ = value;
    return val;
  }

  Value Value::create_array(const Array &value) {
    Value val{};
    val.type_  = Type::Array;
    val.array_ = value;
    return val;
  }

  Value Value::create_object(const Object &value) {
    Value val{};
    val.type_   = Type::Object;
    val.object_ = value;
    return val;
  }

  Value::Value(const Value &other) {
    switch (type_) {
      case Type::Null:
        null_ = {};
        break;
      case Type::Integer:
        integer_ = 0;
        break;
      case Type::Floating:
        floating_ = 0;
        break;
      case Type::Boolean:
        boolean_ = false;
        break;
      case Type::String:
        string_ = {};
        break;
      case Type::Array:
        array_ = {};
        break;
      case Type::Object:
        object_ = {};
        break;
      default:
        break;
    }

    memset(&null_, 0, sizeof(Object));
    type_ = other.type_;
    switch (type_) {
      case Type::Null:
        null_ = {};
        break;
      case Type::Integer:
        integer_ = other.integer_;
        break;
      case Type::Floating:
        floating_ = other.floating_;
        break;
      case Type::Boolean:
        boolean_ = other.boolean_;
        break;
      case Type::String:
        string_ = other.string_;
        break;
      case Type::Array:
        array_ = other.array_;
        break;
      case Type::Object:
        object_ = other.object_;
        break;
      default:
        break;
    }
  }

  Value &Value::operator=(const Value &other) {
    if (this == &other) {
      return *this;
    }

    switch (type_) {
      case Type::Null:
        null_ = {};
        break;
      case Type::Integer:
        integer_ = 0;
        break;
      case Type::Floating:
        floating_ = 0;
        break;
      case Type::Boolean:
        boolean_ = false;
        break;
      case Type::String:
        string_ = {};
        break;
      case Type::Array:
        array_ = {};
        break;
      case Type::Object:
        object_ = {};
        break;
      default:
        break;
    }

    memset(&null_, 0, sizeof(Object));
    type_ = other.type_;
    switch (type_) {
      case Type::Null:
        null_ = {};
        break;
      case Type::Integer:
        integer_ = other.integer_;
        break;
      case Type::Floating:
        floating_ = other.floating_;
        break;
      case Type::Boolean:
        boolean_ = other.boolean_;
        break;
      case Type::String:
        string_ = other.string_;
        break;
      case Type::Array:
        array_ = other.array_;
        break;
      case Type::Object:
        object_ = other.object_;
        break;
      default:
        break;
    }

    return *this;
  }

  Value::~Value() {
    switch (type_) {
      case Type::String:
        string_.~String();
        break;
      case Type::Array:
        array_.~Array();
        break;
      case Type::Object:
        object_.~Object();
        break;
      default:
        break;
    }
  }

  Type Value::type() const {
    return type_;
  }

  bool Value::is_null() const {
    return type_ == Type::Null;
  }

  const i64 *Value::as_integer() const {
    if (type_ != Type::Integer) {
      return nullptr;
    }

    return &integer_;
  }

  const f64 *Value::as_floating() const {
    if (type_ != Type::Floating) {
      return nullptr;
    }

    return &floating_;
  }

  const bool *Value::as_boolean() const {
    if (type_ != Type::Boolean) {
      return nullptr;
    }

    return &boolean_;
  }

  const String *Value::as_string() const {
    if (type_ != Type::String) {
      return nullptr;
    }

    return &string_;
  }

  const Array *Value::as_array() const {
    if (type_ != Type::Array) {
      return nullptr;
    }

    return &array_;
  }

  const Object *Value::as_object() const {
    if (type_ != Type::Object) {
      return nullptr;
    }

    return &object_;
  }

  i64 *Value::as_integer() {
    if (type_ != Type::Integer) {
      return nullptr;
    }

    return &integer_;
  }

  f64 *Value::as_floating() {
    if (type_ != Type::Floating) {
      return nullptr;
    }

    return &floating_;
  }

  bool *Value::as_boolean() {
    if (type_ != Type::Boolean) {
      return nullptr;
    }

    return &boolean_;
  }

  String *Value::as_string() {
    if (type_ != Type::String) {
      return nullptr;
    }

    return &string_;
  }

  Array *Value::as_array() {
    if (type_ != Type::Array) {
      return nullptr;
    }

    return &array_;
  }

  Object *Value::as_object() {
    if (type_ != Type::Object) {
      return nullptr;
    }

    return &object_;
  }

  namespace {
    enum class TokenType : u16 {
      Identifier,
      OpenBrace,
      CloseBrace,
      OpenBracket,
      CloseBracket,
      Comma,
      Colon,
      Null,
      Integer,
      Floating,
      Boolean,
      String,
    };

    struct Token {
      String    string = {};
      TokenType type   = TokenType::Identifier;
    };
  }

  static Vec<Token> tokenize(const String &json) {
    Vec<Token> tokens = {};

    bool   string = false;
    bool   escape = false;
    String str    = {};

    for (usize i = 0; i < json.len(); i++) {
      const char c = json[i];

      if (is_whitespace(c) && !string) {
        continue;
      }

      if (string && c == '\\' && !escape) {
        escape = true;
        continue;
      }

      if (c == '"' && !string && !escape) {
        string = true;
        continue;
      }

      if (c == '"' && string && !escape) {
        string = false;
        tokens.push(Token{.string = str, .type = TokenType::String});
        str = String::empty();
        continue;
      }

      if (c == '"' && string && escape) {
        str.push('"');
        escape = false;
        continue;
      }

      if (c == '\\' && string && escape) {
        str.push('\\');
        escape = false;
        continue;
      }

      if (c == '/' && string && escape) {
        str.push('/');
        escape = false;
        continue;
      }

      if (c == 'b' && string && escape) {
        str.push('\b');
        escape = false;
        continue;
      }

      if (c == 'f' && string && escape) {
        str.push('\f');
        escape = false;
        continue;
      }

      if (c == 'n' && string && escape) {
        str.push('\n');
        escape = false;
        continue;
      }

      if (c == 'r' && string && escape) {
        str.push('\r');
        escape = false;
        continue;
      }

      if (c == 't' && string && escape) {
        str.push('\t');
        escape = false;
        continue;
      }

      if (is_alphanum(c) || c == '.') {
        str.push(c);
        continue;
      }

      if (!(is_alphanum(c) || c == '.' || c == '-' || c == '+') && !str.is_empty()) {
        Maybe boolean  = str.to_bool();
        Maybe integer  = str.to_i64();
        Maybe floating = str.to_f64();

        if (boolean) {
          tokens.push(Token{.string = str, .type = TokenType::Boolean});
        } else if (integer) {
          tokens.push(Token{.string = str, .type = TokenType::Integer});
        } else if (floating) {
          tokens.push(Token{.string = str, .type = TokenType::Floating});
        } else if (str == "null") {
          tokens.push(Token{.string = str, .type = TokenType::Null});
        } else {
          tokens.push(Token{.string = str, .type = TokenType::Identifier});
        }

        str = String::empty();
      }

      if (c == '{') {
        tokens.push(Token{.string = "{", .type = TokenType::OpenBrace});
        continue;
      }
      if (c == '}') {
        tokens.push(Token{.string = "}", .type = TokenType::CloseBrace});
        continue;
      }
      if (c == '[') {
        tokens.push(Token{.string = "[", .type = TokenType::OpenBracket});
        continue;
      }
      if (c == ']') {
        tokens.push(Token{.string = "]", .type = TokenType::CloseBracket});
        continue;
      }
      if (c == ',') {
        tokens.push(Token{.string = ",", .type = TokenType::Comma});
        continue;
      }
      if (c == ':') {
        tokens.push(Token{.string = ":", .type = TokenType::Colon});
        continue;
      }

      str.push(c);
    }

    return tokens;
  }

  static Maybe<usize> parse_array(usize idx, const Vec<Token> &tokens, Array &array);
  static Maybe<usize> parse_object(usize idx, const Vec<Token> &tokens, Object &obj);

  static Maybe<usize> parse_value(usize idx, const Vec<Token> &tokens, Value &value) {
    usize       i              = idx;
    const auto &[string, type] = tokens[i];
    if (type == TokenType::String) {
      value = Value::create_string(tokens[i].string);
      i++;
      return i;
    }

    if (type == TokenType::Integer) {
      value = Value::create_integer(*tokens[i].string.to_i64());
      i++;
      return i;
    }

    if (type == TokenType::Floating) {
      value = Value::create_floating(*tokens[i].string.to_f64());
      i++;
      return i;
    }

    if (type == TokenType::Boolean) {
      value = Value::create_boolean(*tokens[i].string.to_bool());
      i++;
      return i;
    }

    if (type == TokenType::Null) {
      value = Value::create_null();
      i++;
      return i;
    }

    if (type == TokenType::OpenBrace) {
      Object obj{};
      Maybe  maybe = parse_object(i, tokens, obj);
      if (!maybe) {
        return {};
      }

      i     = *maybe;
      value = Value::create_object(obj);
    }

    if (type == TokenType::OpenBracket) {
      Array array{};
      Maybe maybe = parse_array(i, tokens, array);
      if (!maybe) {
        return {};
      }

      i     = *maybe;
      value = Value::create_array(array);
    }

    return i;
  }

  static Maybe<usize> parse_kv_pair(usize idx, const Vec<Token> &tokens, String &key, Value &value) {
    usize i = idx;
    if (tokens[i].type != TokenType::String) {
      LOG_ERR("JSON parsing failed: %s", "Expected string key");
      return {};
    }

    key = tokens[i].string;

    i++;
    if (tokens[i].type != TokenType::Colon) {
      LOG_ERR("JSON parsing failed: %s", "Expected colon after key");
      return {};
    }

    i++;
    Maybe maybe = parse_value(i, tokens, value);
    if (!maybe) {
      return {};
    }

    i = *maybe;
    return i;
  }

  Maybe<usize> parse_array(usize idx, const Vec<Token> &tokens, Array &array) {
    usize i = idx;
    if (tokens[i].type != TokenType::OpenBracket) {
      LOG_ERR("JSON parsing failed: %s", "Expected '['");
      return {};
    }

    i++;
    while (true) {
      Value value{};

      Maybe maybe = parse_value(i, tokens, value);
      if (!maybe) {
        return {};
      }

      array.push(value);

      i = *maybe;
      if (tokens[i].type == TokenType::Comma) {
        i++;
        continue;
      }

      if (tokens[i].type == TokenType::CloseBracket) {
        i++;
        return i;
      }

      LOG_ERR("JSON parsing failed: %s", "Expected ',' or ']'");
      return {};
    }
  }

  Maybe<usize> parse_object(usize idx, const Vec<Token> &tokens, Object &obj) {
    usize i = idx;
    if (tokens[i].type != TokenType::OpenBrace) {
      LOG_ERR("JSON parsing failed: %s", "Expected '{'");
      return {};
    }

    i++;
    while (true) {
      String key{};
      Value  value{};

      Maybe maybe = parse_kv_pair(i, tokens, key, value);
      if (!maybe) {
        return {};
      }

      obj[key] = value;

      i = *maybe;
      if (tokens[i].type == TokenType::Comma) {
        i++;
        continue;
      }

      if (tokens[i].type == TokenType::CloseBrace) {
        i++;
        return i;
      }

      LOG_ERR("JSON parsing failed: %s", "Expected ',' or '}'");
      return {};
    }
  }

  static Maybe<Value> parse_tokens(const Vec<Token> &tokens) {
    Value value = Value::create_object();

    const Maybe maybe = parse_object(0, tokens, *value.as_object());
    if (!maybe) {
      return {};
    }

    return value;
  }

  static String array_to_string(const Array &array, u32 indent = 0);
  static String object_to_string(const Object &obj, u32 indent = 0);

  static String value_to_string(const Value &value, u32 indent) {
    switch (value.type()) {
      case Type::Null:
        return "null";
      case Type::Boolean:
        return *value.as_boolean() ? "true" : "false";
      case Type::Integer:
        return Flock::to_string(*value.as_integer());
      case Type::Floating:
        return Flock::to_string(*value.as_floating());
      case Type::String: {
        String string = *value.as_string();
        string.insert(0, '\"');
        string.push('\"');
        return string;
      }
      case Type::Array:
        return array_to_string(*value.as_array(), indent);
      case Type::Object:
        return object_to_string(*value.as_object(), indent);
      default:
        return "";
    }
  }

  String array_to_string(const Array &array, u32 indent) {
    String str = "[\n";

    for (const auto &val: array) {
      for (usize i = 0; i < indent + 2; i++) {
        str.push(' ');
      }

      str.push_str(value_to_string(val, indent + 2));
      str.push_str(",\n");
    }

    if (!array.is_empty()) {
      str.truncate(str.len() - 2);
      str.push('\n');
    }

    for (usize i = 0; i < indent; i++) {
      str.push(' ');
    }

    str.push_str("]");
    return str;
  }

  String object_to_string(const Object &obj, u32 indent) {
    String str = "{\n";

    for (const auto &[key, val]: obj) {
      for (usize i = 0; i < indent + 2; i++) {
        str.push(' ');
      }

      str.push('\"');
      str.push_str(key);
      str.push('\"');
      str.push_str(": ");
      str.push_str(value_to_string(val, indent + 2));
      str.push_str(",\n");
    }

    if (!obj.is_empty()) {
      str.truncate(str.len() - 2);
      str.push('\n');
    }

    for (usize i = 0; i < indent; i++) {
      str.push(' ');
    }

    str.push_str("}");
    return str;
  }

  Maybe<Value> parse(const String &json) {
    const Vec<Token> tokens = tokenize(json);
    return parse_tokens(tokens);
  }

  String to_string(const Value &value) {
    return value_to_string(value, 0);
  }
}
