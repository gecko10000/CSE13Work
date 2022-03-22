#include "mathlib.h"

#include <math.h> // oh no!
#include <stdint.h>

#define EPSILON 1e-14

double Exp(double x) { // adapted from example on assignment
    double trm = 1.0;
    double sum = trm;
    int k = 1;
    while (trm > EPSILON) {
        trm *= Abs(x) / k; // calculate the same no matter whether it's positive or negative
        sum += trm;
        k++;
    }
    return x > 0 ? sum : 1 / sum; // return the reciprocal if it's negative
}

double normalize(double x, double interval) {
    return x - (int) (x / interval) * interval; // like modulus, but for doubles
}

double Sin(double x) { // adapted from assignment
    x = normalize(x, 2 * M_PI); // make it easier on our series
    double s = 1, v = x, t = x, k = 3;
    while (Abs(t) > EPSILON) {
        t = t * (x * x) / ((k - 1) * k);
        s = -s;
        v += s * t;
        k += 2;
    }
    return v;
}

double Cos(double x) { // also adapted from assignment
    x = normalize(x, 2 * M_PI);
    double s = 1, v = 1, t = 1, k = 2;
    while (Abs(t) > EPSILON) {
        t = t * (x * x) / ((k - 1) * k);
        s = -s;
        v += s * t;
        k += 2;
    }
    return v;
}

double Sqrt(double x) {
    if (x < 0) {
        return NAN; // early return for invalid input
    }
    double z = 0, y = 1;

    // dividing by 4 to simplify series
    long factoredOut = 1;
    while (x > 1) {
        x /= 4;
        factoredOut *= 2;
    }

    // adapted from assignment
    while (Abs(y - z) > EPSILON) {
        z = y;
        y = (z + x / z) / 2.0;
    }
    return y * factoredOut;
}

double Log(double x) {
    if (x <= 0) { // undefined for these values
        return x < 0 ? NAN : -INFINITY; // recreating math.exp's behavior
    }
    double y = 1;
    double p = Exp(y);

    // dividing x by e until x is within [1, e)
    long factoredOut = 0;
    double e = Exp(1);
    while (x > e) {
        x /= e;
        factoredOut++; // log(e) == 1
    }
    // adapted from assignment
    while (Abs(p - x) > EPSILON) {
        y += x / p - 1;
        p = Exp(y);
    }
    return y + factoredOut;
}

// helper function for readability
double getX(double j, double a, double h) {
    return a + j * h;
}

double integrate(double (*f)(double x), double a, double b, uint32_t n) {
    double h = (b - a) / n;
    double result = (*f)(getX(0, a, h));
    uint32_t max = n / 2;

    // sums
    for (uint32_t j = 1; j <= max; j++) {
        if (j != max) {
            result += 2 * (*f)(getX(2 * j, a, h));
        }
        result += 4 * (*f)(getX(2 * j - 1, a, h));
    }
    result += (*f)(getX(n, a, h));
    return result * h / 3;
}
