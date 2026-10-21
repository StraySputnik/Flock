#include "debug/log.hpp"

#include "time/time.hpp"

namespace Flock::Debug {
  HashMap<String, Logger> Logger::s_loggers_ = {};

  String Logger::format(const StringSlice slice, const char *name, const LogLevel level) {
    String result = {};

    auto [year, mon, mday, wday, hour, min, sec] = Time::DateAndTime::local_now();
    char buf[8];

    bool placeholder = false;
    for (usize i = 0; i < slice.len(); i++) {
      const char ch = slice[i];
      if (ch == '%' && !placeholder) {
        placeholder = true;
        continue;
      }

      if (ch == '%' && placeholder) {
        placeholder = false;
        result.push('%');
        continue;
      }

      if (!placeholder) {
        result.push(ch);
        continue;
      }

      switch (ch) {
        case 'Y':
          snprintf(buf, sizeof(buf), "%d", year);
          result.push_str(buf);
          break;
        case 'm':
          snprintf(buf, sizeof(buf), "%02d", mon + 1);
          result.push_str(buf);
          break;
        case 'd':
          snprintf(buf, sizeof(buf), "%02d", mday + 1);
          result.push_str(buf);
          break;
        case 'H':
          snprintf(buf, sizeof(buf), "%02d", hour);
          result.push_str(buf);
          break;
        case 'M':
          snprintf(buf, sizeof(buf), "%02d", min);
          result.push_str(buf);
          break;
        case 'S':
          snprintf(buf, sizeof(buf), "%02d", sec);
          result.push_str(buf);
          break;
        case 'l':
          result.push_str(log_level_to_c_str(level));
          break;
        case 'N':
          result.push_str(name);
          break;
        case 'c': {
          char c = ' ';
          switch (level) {
            case Trace: c = 't';
              break;
            case Debug: c = 'd';
              break;
            case Info: c = 'i';
              break;
            case Warning: c = '?';
              break;
            case Error: c = '!';
              break;
            case Fatal: c = 'f';
              break;
            default: c = 'u';
              break;
          }

          result.push(c);
          break;
        }
        default:
          ASSERT(false, "Invalid format specifier");
      }

      placeholder = false;
    }

    return result;
  }
}
