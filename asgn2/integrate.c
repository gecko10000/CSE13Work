#include "functions.h"
#include "mathlib.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// typedef explanation from https://www.geeksforgeeks.org/returning-a-function-pointer-from-a-function-in-c-cpp/

typedef double (*fun)(double); // define fun type for readability and reuse

void usage(char *arg0, char *names[]) { // printed when invalid input is given
    char *format = "-%-4c  %-15s%-40s\n";
    char *funcFormat = "-%-4c%17s%-40s\n";
    printf("SYNOPSIS\n"
           "\t%s [OPTIONS]\n",
        arg0);

    printf("\n"
           "DESCRIPTION\n"
           "\tThis program calculates integrals of functions\n\tusing the Composite Simpson's 1/3 "
           "Rule.\n");

    printf("\n"
           "USAGE\n");
    printf("%4s  %-15s%-40s\n", "Flag", "Argument", "Formula/Use");
    printf("\n");
    for (int i = 'a'; i <= 'j'; i++) {
        printf(funcFormat, i, "", names[i - 'a']);
    }
    printf("\n");
    printf(format, 'n', "partitions", "Set the number of partitions to split the interval into");
    printf(format, 'p', "low", "Set the low end of the calculated range");
    printf(format, 'q', "high", "Set the high end of the calculated range");
    printf(format, 'H', "", "Display this usage text");
    exit(1);
}

int main(int argc, char *argv[]) {
    fun functions[] = { a, b, c, d, e, f, g, h, i, j };
    char *names[] = { "sqrt(1 - x^4)", "1 / log(x)", "e^(-x^2)", "sin(x^2)", "cos(x^2)",
        "log(log(x))", "sin(x) / x", "e^(-x) / x", "e^(e^x)", "sqrt(sin^2(x) + cos^2(x))" };
    fun f = NULL;

    // to better understand getopt, i used this
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    int option;
    uint32_t n = 100;
    double low = NAN;
    double high = NAN;
    int functionIndex;
    while ((option = getopt(argc, argv, "abcdefghijn:p:q:H")) != -1) {
        if (option >= 'a' && option <= 'j') { // so glad chars are also numbers
            functionIndex = option - 'a';
            f = functions[functionIndex];
            continue;
        }
        switch (option) {
        case 'n': n = strtoul(optarg, NULL, 10); break;
        case 'p': low = atof(optarg); break;
        case 'q': high = atof(optarg); break;
        default: usage(argv[0], names); // -H
        }
    }
    if (f == NULL || isnan(low) || isnan(high)) { // function, low, and high were not specified
        usage(argv[0], names);
    }

    printf("%s,%f,%f,%d\n", names[functionIndex], low, high, n);
    for (uint32_t i = 2; i <= n; i += 2) {
        printf("%" PRIu32 ",%.15f\n", i,
            integrate(f, low, high, i)); // call integrate and print the output
    }
    return 0;
}
