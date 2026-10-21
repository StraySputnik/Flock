#include "time/time.hpp"

#include <ctime>

namespace Flock::Time {
  u8 days_in_month(const u8 month, const bool leap_year) {
    switch (month) {
      case JANUARY:
        return 31;
      case FEBRUARY:
        return leap_year ? 29 : 28;
      case MARCH:
        return 31;
      case APRIL:
        return 30;
      case MAY:
        return 31;
      case JUNE:
        return 30;
      case JULY:
        return 31;
      case AUGUST:
        return 31;
      case SEPTEMBER:
        return 30;
      case OCTOBER:
        return 31;
      case NOVEMBER:
        return 30;
      case DECEMBER:
        return 31;
      default:
        PANIC();
    }

    UNREACHABLE();
  }

  bool is_leap_year(const i32 year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
  }

  i32 days_in_year(const i32 year) {
    return is_leap_year(year) ? 366 : 365;
  }

  i64 unix_now() {
    return time(nullptr);
  }

  DateAndTime DateAndTime::local_now() {
    return local(unix_now());
  }

  DateAndTime DateAndTime::utc_now() {
    return utc(unix_now());
  }

  DateAndTime DateAndTime::local(const i64 unix_time) {
    tm time{};
#ifdef PLATFORM_WINDOWS
    localtime_s(&time, &unix_time);
#else
    localtime_r(&unix_time, &time);
#endif

    return {
      .year = static_cast<u16>(time.tm_year + 1900),
      .mon  = static_cast<u8>(time.tm_mon),
      .mday = static_cast<u8>(time.tm_mday - 1),
      .wday = static_cast<u8>(time.tm_wday),
      .hour = static_cast<u8>(time.tm_hour),
      .min  = static_cast<u8>(time.tm_min),
      .sec  = static_cast<u8>(time.tm_sec),
    };
  }

  DateAndTime DateAndTime::utc(const i64 unix_time) {
    tm time{};
#ifdef PLATFORM_WINDOWS
    gmtime_s(&time, &unix_time);
#else
    gmtime_r(&unix_time, &time);
#endif

    return {
      .year = static_cast<u16>(time.tm_year + 1900),
      .mon  = static_cast<u8>(time.tm_mon),
      .mday = static_cast<u8>(time.tm_mday - 1),
      .wday = static_cast<u8>(time.tm_wday),
      .hour = static_cast<u8>(time.tm_hour),
      .min  = static_cast<u8>(time.tm_min),
      .sec  = static_cast<u8>(time.tm_sec),
    };
  }

  i64 DateAndTime::unix_time() const {
    i64 unix_time = 0;

    i32 days = 0;
    for (i32 i = 1970; i < year; i++) {
      days += days_in_year(i);
    }

    for (i32 i = 0; i < mon; i++) {
      days += days_in_month(i, is_leap_year(year));
    }

    days += mday;

    unix_time += days * 86400LL;
    unix_time += static_cast<i64>(hour) * 3600;
    unix_time += static_cast<i64>(min) * 60;
    unix_time += sec;

    return unix_time;
  }
}
