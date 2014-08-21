#pragma once

#define max(a,b) \
  ({__typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define min(a,b) \
  ({__typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

typedef unsigned int uint;

double modulo(double a, double n);

int round_d(double d);

double square(double a);

double sqrt(double num);
