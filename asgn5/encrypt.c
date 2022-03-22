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
    printf("   Encrypts the given file with a public key.\n\n");

    printf("USAGE\n");
    printf("   %s [-i input] [-o output] [-n pub] [-vh]\n\n", arg0);

    printf("OPTIONS\n");
    printf(HELP_FMT, 'h', "", "Shows this message with help and usage.");
    printf(HELP_FMT, 'i', "input", "Sets the input file to encrypt (default stdin).");
    printf(HELP_FMT, 'o', "output",
        "Sets the output file to write the encrypted data to (default stdout).");
    printf(HELP_FMT, 'n', "pbfile",
        "Specifies the public key file to use for encoding (default rsa.pub).");
    printf(HELP_FMT, 'v', "", "Enables verbose output for the retrieved values.");
    exit(1);
    return;
}

int main(int argc, char *argv[]) {
    // defaults
    FILE *input = stdin;
    FILE *output = stdout;
    char *pbname = "rsa.pub";
    bool verbose = false;

    // parse flags
    int option;
    while ((option = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (option) {
        case 'i': input = fopen(optarg, "r"); break;
        case 'o': output = fopen(optarg, "w"); break;
        case 'n': pbname = optarg; break;
        case 'v': verbose = true; break;
        case 'h':
        default: usage(argv[0]);
        }
    }

    // invalid input or output was specified
    if (input == NULL || output == NULL) {
        printf("%s %s %s\n", "Invalid", input == NULL ? "input" : "output", "file.");
        usage(argv[0]);
    }

    // open public key to read
    FILE *pbfile = fopen(pbname, "r");
    if (pbfile == NULL) {
        printf("%s\n", "Invalid public key file.");
        usage(argv[0]);
    }
    mpz_t n, e, s, u;
    mpz_inits(n, e, s, u, NULL);
    // create username string
    char username[256] = {
        0
    }; // from Omar in Discord https://discord.com/channels/926211705766305842/926211706382868587/939780597386194974
    // read public file into variables
    rsa_read_pub(n, e, s, username, pbfile);
    // print verbose output
    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf(VERBOSE_FMT, 's', mpz_sizeinbase(s, 2), s);
        gmp_printf(VERBOSE_FMT, 'n', mpz_sizeinbase(n, 2), n);
        gmp_printf(VERBOSE_FMT, 'e', mpz_sizeinbase(e, 2), e);
    }

    // convert username to mpz_t
    mpz_set_str(u, username, 62);
    // verify signature
    if (!rsa_verify(u, s, e, n)) {
        printf("Signature could not be verified.\n");
        usage(argv[0]);
    }
    // encrypt given file and write to output
    rsa_encrypt_file(input, output, n, e);

    // clean up
    fclose(input);
    fclose(output);
    fclose(pbfile);
    mpz_clears(n, e, s, u, NULL);
    return 0;
}
