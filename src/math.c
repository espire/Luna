#include "math.h"

double modulo(double a, double n) {
  return a - n * ((int) (a / n));
}

int round_d(double d) {
  if (d - (int) d >= 0.5) {
    return (int) d + 1;
  } else {
    return (int) d;
  }
}

double square(double a) {
  return a * a;
}

double sqrt(double num) {
  float a, p, e = 0.001, b;

  a = num;
  p = a * a;
  while(p - num >= e)
  {
    b = (a + (num / a)) / 2;
    a = b;
    p = a * a;
  }
  return a;
}
