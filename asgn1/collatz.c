#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

uint64_t f(uint64_t n) {
    return (n & 0x1) ? 3 * n + 1 : n / 2;
}

#define OPTIONS "hn:r:"

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Prints the Collatz sequence.\n"
        "\n"
        "USAGE\n"
        "   %s [-hn:r:] [-n start] [-r seed]\n"
        "\n"
        "OPTIONS\n"
        "   -h         display program help and usage.\n"
        "   -n start   deterministic stating point.\n"
        "   -r seed    deterministic random stating point.\n",
        exec);
}

int main(int argc, char **argv) {

    uint32_t seed = time(0); // Default (no options) is random based on time.
    srandom(seed);
    uint32_t start = random();

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'n':
            start = (uint32_t) strtoul(optarg, NULL, 10); // Explicit starting point
            break;
        case 'r':
            seed = (uint32_t) strtoul(optarg, NULL,
                10); // Deterministic random starting point
            srandom(seed);
            start = random();
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    for (uint64_t n = start; n != 1; n = f(n)) {
        printf("%" PRIu64 "\n", n);
    }
    printf("1\n");

    return 0;
}
