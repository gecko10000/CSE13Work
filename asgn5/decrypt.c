#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

#include "rsa.h"

#define HELP_FMT    "   -%c %-12s %-s\n"
#define VERBOSE_FMT "%c (%" PRIu64 " bits) = %Zd\n"

gmp_randstate_t state;

// Shows program usage and exits when
// usage is requested or an error occurs.
//
// arg0: the command used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Decrypts the given file with a private key.\n\n");

    printf("USAGE\n");
    printf("   %s [-i input] [-o output] [-n priv] [-vh]\n\n", arg0);

    printf("OPTIONS\n");
    printf(HELP_FMT, 'h', "", "Shows this message with help and usage.");
    printf(HELP_FMT, 'i', "input", "Sets the input file to decrypt (default stdin).");
    printf(HELP_FMT, 'o', "output",
        "Sets the output file to write the decrypted data to (default stdout).");
    printf(HELP_FMT, 'n', "pvfile",
        "Specifies the private key file to use for decryption (default rsa.priv).");
    printf(HELP_FMT, 'v', "", "Enables verbose output for printing retrieved private key values.");
    exit(1);
    return;
}

int main(int argc, char *argv[]) {
    // defaults
    FILE *input = stdin;
    FILE *output = stdout;
    char *pvname = "rsa.priv";
    bool verbose = false;

    // flag processing
    int option;
    while ((option = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (option) {
        case 'i': input = fopen(optarg, "r"); break;
        case 'o': output = fopen(optarg, "w"); break;
        case 'n': pvname = optarg; break;
        case 'v': verbose = true; break;
        case 'h':
        default: usage(argv[0]);
        }
    }

    // invalid input or output handling
    if (input == NULL || output == NULL) {
        printf("%s %s %s\n", "Invalid", input == NULL ? "input" : "output", "file.");
        usage(argv[0]);
    }

    // open private key for reading
    FILE *pvfile = fopen(pvname, "r");
    if (pvfile == NULL) {
        printf("Invalid private key %s.\n", pvname);
        usage(argv[0]);
    }
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    // read private key to variables
    rsa_read_priv(n, d, pvfile);

    // verbose output
    if (verbose) {
        gmp_printf(VERBOSE_FMT, 'n', mpz_sizeinbase(n, 2), n);
        gmp_printf(VERBOSE_FMT, 'd', mpz_sizeinbase(d, 2), d);
    }

    // decrypt input file to output
    rsa_decrypt_file(input, output, n, d);

    // clean up
    fclose(pvfile);
    fclose(input);
    fclose(output);
    mpz_clears(n, d, NULL);
    return 0;
}
