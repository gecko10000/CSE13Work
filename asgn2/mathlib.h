#pragma once

#include <stdint.h>

#define EPSILON 1e-14

static inline double Abs(double x) {
    return x < 0.0 ? -x : x;
}

double Exp(double x);

double Sin(double x);

double Cos(double x);

double Sqrt(double x);

double Log(double x);

double integrate(double (*f)(double x), double a, double b, uint32_t n);
