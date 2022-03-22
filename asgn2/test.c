#include "functions.h"
#include "mathlib.h"

#include <math.h>
#include <stdio.h>

#define NAME_FORMAT "%10s%12s%29s%29s\n"
#define NUM_FORMAT  "%10s%12s%29.9f%29.9f\n"

double (*f1)(double) = exp;
double (*f2)(double) = Exp;

void test(double input, char *inputString, char *outputString) {
    printf(NUM_FORMAT, inputString, outputString, (*f1)(input), (*f2)(input));
}

int main(void) {

    printf("exp tests:\n" NAME_FORMAT, "input", "output", "exp", "Exp");
    test(0, "0", "1");
    test(1, "1", "e");
    test(40, "40", "a lot");

    f1 = sin;
    f2 = Sin;

    printf("\nsin tests:\n" NAME_FORMAT, "input", "output", "sin", "Sin");
    test(0, "0", "0");
    test(M_PI / 3, "pi/3", "sqrt(3)/2");
    test(M_PI / 2, "pi/2", "1");
    test(M_PI, "pi", "0");
    test(3 * M_PI / 2, "3pi/2", "-1");
    test(401 * M_PI / 2, "200.5pi", "1");
    test(1000 * M_PI / 3, "1000pi/3", "-sqrt(3)/2");

    f1 = cos;
    f2 = Cos;

    printf("\ncos tests:\n" NAME_FORMAT, "input", "output", "cos", "Cos");
    test(0, "0", "1");
    test(M_PI / 3, "pi/3", "0.5");
    test(M_PI / 2, "pi/2", "0");
    test(M_PI, "pi", "-1");
    test(3 * M_PI / 2, "3pi/2", "0");
    test(201 * M_PI, "201pi", "-1");
    test(1000 * M_PI / 3, "1000pi/3", "-0.5");

    f1 = sqrt;
    f2 = Sqrt;

    printf("\nsqrt tests:\n" NAME_FORMAT, "input", "output", "sqrt", "Sqrt");
    test(0, "0", "0");
    test(4, "4", "2");
    test(169, "169", "13");
    test(1000000, "1,000,000", "1,000");
    test(-1, "-1", "NaN");

    f1 = log;
    f2 = Log;

    printf("\nlog tests:\n" NAME_FORMAT, "input", "output", "log", "Log");
    test(0, "0", "-infinity");
    test(exp(4), "e^4", "4");
    test(exp(100), "e^100", "100");
    test(-1, "-1", "NaN");
    test(exp(0.01), "e^0.01", "0.01");

    return 0;
}
