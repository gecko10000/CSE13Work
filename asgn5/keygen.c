#include <stdint.h>
#include <gmp.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define VERBOSE_FMT "%c (%" PRIu64 " bits) = %Zd\n"
#define HELP_FMT    "   -%c %-12s %-s\n"

// Shows program usage and exits when
// usage is requested or an error occurs.
//
// arg0: the command used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Creates an RSA public and private key pair.\n\n");

    printf("USAGE\n");
    printf("   %s [-b bits] [-i iterations] [-n pub] [-d priv] [-s seed] [-vh]\n\n", arg0);

    printf("OPTIONS\n");
    printf(HELP_FMT, 'h', "", "Shows this message with help and usage.");
    printf(HELP_FMT, 'b', "bits", "Sets the minimum number of bits to generate primes with.");
    printf(HELP_FMT, 'i', "iterations",
        "Sets the number of times to test primality with the Miller-Rabin algorithm.");
    printf(HELP_FMT, 'n', "pbfile", "Sets the file to output the public key to (default rsa.pub).");
    printf(
        HELP_FMT, 'd', "pvfile", "Sets the file to output the private key to (default rsa.priv).");
    printf(HELP_FMT, 's', "seed", "Sets the seed for the random number generation.");
    printf(HELP_FMT, 'v', "", "Enables verbose output to print the generated values.");
    exit(1);
    return;
}

// Opens the specified file for writing.
// Returns: the opened file pointer, or
// exits if it wasn't able to
//
// filename: the name of the file to open
FILE *open_write(char *filename) {
    FILE *file = fopen(filename, "w");
    if (errno != 0) {
        printf("%s\n", strerror(errno));
        exit(errno);
    }
    return file;
}

int main(int argc, char *argv[]) {
    // defaults
    uint32_t bits = 256;
    uint32_t iterations = 50;
    char *pbname = "rsa.pub";
    char *pvname = "rsa.priv";
    uint64_t seed = time(NULL);
    bool verbose = false;

    // process flags
    int option;
    while ((option = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
        switch (option) {
        case 'b': bits = strtoul(optarg, NULL, 10); break;
        case 'i': iterations = strtoul(optarg, NULL, 10); break;
        case 'n': pbname = optarg; break;
        case 'd': pvname = optarg; break;
        case 's': seed = strtoul(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        case 'h':
        default: usage(argv[0]);
        }
    }
    // must have > 0 bits
    if (bits <= 0) {
        usage(argv[0]);
    }

    // open files for writing
    FILE *pbfile = open_write(pbname);
    FILE *pvfile = open_write(pvname);

    // ensure proper private key file permissions
    fchmod(fileno(pvfile), 0600);

    // seed pseudorandom number generators
    randstate_init(seed);

    // create public and private keys
    mpz_t p, q, n, e, d, u, s;
    mpz_inits(p, q, n, e, d, u, s, NULL);
    rsa_make_pub(p, q, n, e, bits, iterations);
    rsa_make_priv(d, e, p, q);

    // get username
    char *username = getenv("USER"); // from running `$ man getenv` and `$ printenv`

    // sign username
    mpz_set_str(u, username, 62);
    rsa_sign(s, u, d, n);

    // write keys to respective files
    rsa_write_pub(n, e, s, username, pbfile);
    rsa_write_priv(n, d, pvfile);

    // print verbose output
    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf(VERBOSE_FMT, 's', mpz_sizeinbase(s, 2), s);
        gmp_printf(VERBOSE_FMT, 'p', mpz_sizeinbase(p, 2), p);
        gmp_printf(VERBOSE_FMT, 'q', mpz_sizeinbase(q, 2), q);
        gmp_printf(VERBOSE_FMT, 'n', mpz_sizeinbase(n, 2), n);
        gmp_printf(VERBOSE_FMT, 'e', mpz_sizeinbase(e, 2), e);
        gmp_printf(VERBOSE_FMT, 'd', mpz_sizeinbase(d, 2), d);
    }

    // clean up
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, n, e, d, u, s, NULL);
    return 0;
}
