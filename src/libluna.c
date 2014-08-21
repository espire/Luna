#include "libluna.h"

#include "math.h"

int julian_day_number(int year, int month, int day) {
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jdn =
      day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  return jdn;
}

double moon_age_from_time(int year, int month, int day, int hours) {
  int known_moon = julian_day_number(1900, 1, 1);
  int this_day = julian_day_number(year, month, day);

  double days = this_day - known_moon;
  return modulo(days, SYNODIC_MONTH);
}
