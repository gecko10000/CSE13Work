#include "mathlib.h"

double a(double x) {
    return Sqrt(1.0 - x * x * x * x);
}

double b(double x) {
    return 1.0 / Log(x);
}

double c(double x) {
    return Exp(-x * x);
}

double d(double x) {
    return Sin(x * x);
}

double e(double x) {
    return Cos(x * x);
}

double f(double x) {
    return Log(Log(x));
}

double g(double x) {
    return Sin(x) / x;
}

double h(double x) {
    return Exp(-x) / x;
}

double i(double x) {
    return Exp(Exp(x));
}

double j(double x) {
    return Sqrt(Sin(x) * Sin(x) + Cos(x) * Cos(x));
}
