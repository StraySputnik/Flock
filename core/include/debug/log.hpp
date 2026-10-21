#pragma once

#include "common.hpp"
#include "collect/hash_map.hpp"
#include "collect/string.hpp"
#include "debug/print.hpp"

namespace Flock::Debug {
  enum LogLevel : u8 {
    None    = 0b000000,
    Trace   = 0b000001,
    Debug   = 0b000010,
    Info    = 0b000100,
    Warning = 0b001000,
    Error   = 0b010000,
    Fatal   = 0b100000,
    All     = 0b111111,
  };

  inline const char *log_level_to_c_str(const LogLevel level) {
    switch (level) {
      case Trace:
        return "TRC";
      case Debug:
        return "DBG";
      case Info:
        return "INF";
      case Warning:
        return "WRN";
      case Error:
        return "ERR";
      case Fatal:
        return "FTL";
      default:
        return "UNK";
    }
  }

  struct FLK_API LoggerConfig {
    String log_format = String::from_c_str("%N: [%Y-%m-%d %H:%M:%S] (%c) ");
    u8     log_level  = All;
  };

  class FLK_API Logger {
    static HashMap<String, Logger> s_loggers_;

    LoggerConfig config_ = {};
    String       name_   = {};

    static String format(StringSlice slice, const char *name, LogLevel level);

  public:
    static Logger &create(const char *name, const LoggerConfig &config) {
      ASSERT(!s_loggers_.has(String::from_c_str(name)), "Logger already exists");

      s_loggers_[String::from_c_str(name)].config_ = config;
      s_loggers_[String::from_c_str(name)].name_   = String::from_c_str(name);
      return s_loggers_[String::from_c_str(name)];
    }

    static Logger &create(const StringSlice name, const LoggerConfig &config) {
      ASSERT(!s_loggers_.has(name.to_string()), "Logger already exists");

      s_loggers_[name.to_string()].config_ = config;
      s_loggers_[name.to_string()].name_   = name.to_string();
      return s_loggers_[name.to_string()];
    }

    static void remove(const StringSlice name) {
      s_loggers_.remove(name.to_string());
    }

    static Logger *get(const char *name) {
      return s_loggers_.get(String::from_c_str(name));
    }

    static Logger *get(const StringSlice name) {
      return s_loggers_.get(name.to_string());
    }

    void configure(const LoggerConfig &config) {
      config_ = config;
    }

    template<typename... Args>
    void log(const LogLevel level, const char *fmt, Args... args) {
      if (!(static_cast<u8>(level) & config_.log_level)) {
        return;
      }

      String log = format(config_.log_format, name_.c_str(), level);
      log.push_str(fmt);
      println(log.c_str(), args...);
    }

    template<typename... Args>
    void log_trc(const char *fmt, Args... args) {
      return log(Trace, fmt, args...);
    }

    template<typename... Args>
    void log_dbg(const char *fmt, Args... args) {
      return log(Debug, fmt, args...);
    }

    template<typename... Args>
    void log_inf(const char *fmt, Args... args) {
      return log(Info, fmt, args...);
    }

    template<typename... Args>
    void log_wrn(const char *fmt, Args... args) {
      return log(Warning, fmt, args...);
    }

    template<typename... Args>
    void log_err(const char *fmt, Args... args) {
      return log(Error, fmt, args...);
    }

    template<typename... Args>
    void log_ftl(const char *fmt, Args... args) {
      return log(Fatal, fmt, args...);
    }
  };
}
