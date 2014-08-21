#pragma once

#include "math.h"

#define SYNODIC_MONTH 29.530588853

int julian_day_number(int year, int month, int day);

double moon_age_from_time(int year, int month, int day, int hours);
