#pragma once

#include "common.hpp"

namespace Flock::Time {
  // Months
  constexpr u8 JANUARY   = 0;
  constexpr u8 FEBRUARY  = 1;
  constexpr u8 MARCH     = 2;
  constexpr u8 APRIL     = 3;
  constexpr u8 MAY       = 4;
  constexpr u8 JUNE      = 5;
  constexpr u8 JULY      = 6;
  constexpr u8 AUGUST    = 7;
  constexpr u8 SEPTEMBER = 8;
  constexpr u8 OCTOBER   = 9;
  constexpr u8 NOVEMBER  = 10;
  constexpr u8 DECEMBER  = 11;

  // Days
  constexpr u8 SUNDAY    = 0;
  constexpr u8 MONDAY    = 1;
  constexpr u8 TUESDAY   = 2;
  constexpr u8 WEDNESDAY = 3;
  constexpr u8 THURSDAY  = 4;
  constexpr u8 FRIDAY    = 5;
  constexpr u8 SATURDAY  = 6;

  constexpr const char *WEEK_DAYS[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  u8 FLK_API   days_in_month(u8 month, bool leap_year = false);
  bool FLK_API is_leap_year(i32 year);
  i32 FLK_API  days_in_year(i32 year);
  FLK_API i64  unix_now();

  struct FLK_API DateAndTime {
    u16 year = 0;
    u8  mon  = 0;
    u8  mday = 0;
    u8  wday = 0;
    u8  hour = 0;
    u8  min  = 0;
    u8  sec  = 0;

    static DateAndTime local_now();
    static DateAndTime utc_now();
    static DateAndTime local(i64 unix_time);
    static DateAndTime utc(i64 unix_time);

    [[nodiscard]] i64 unix_time() const;
  };
}
