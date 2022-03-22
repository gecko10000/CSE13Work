#include <stdlib.h>

#include "rsa.h"
#include "numtheory.h"

extern gmp_randstate_t state;

// Gets the least common multiple of a
// and b and sets it to m.
// Done by calculating the GCD and multiplying
// by the divided numbers
// i.e. 15 and 20; GCD is 5; LCM is 5 * 15/5 * 20/5 = 60
// (although, one of these fractions can be cancelled out by the GCD)
//
// m: the output (LCM)
// a: the first number
// b: the second number
static void lcm(mpz_t m, mpz_t a, mpz_t b) {
    gcd(m, a, b); // get the GCD
    mpz_fdiv_q(m, a, m); // gets one of the fractions
    mpz_mul(m, m, b); // multiplies by LCM * fraction, which is just the number
    return;
}

// Creates parts of an RSA public key
// and stores them in p, q, n, and e
//
// p: the first prime to create
// q: the second prime to create
// n: the product of the primes
// e: the public exponent to use
// nbits: the number of bits to split across p and q
// iters: the number of tests to do to ensure a number is prime
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    // set pbits to [nbits/4, (3*nbits)/4)
    uint64_t pbits = random() % (nbits / 2) + nbits / 4;
    // rest goes to qbits
    uint64_t qbits = nbits - pbits;
    make_prime(p, pbits, iters);
    make_prime(q, qbits, iters);
    mpz_mul(n, p, q);

    mpz_t totient, p_minus_1, q_minus_1, denom;
    mpz_inits(totient, p_minus_1, q_minus_1, denom, NULL);
    mpz_sub_ui(p_minus_1, p, 1);
    mpz_sub_ui(q_minus_1, q, 1);
    lcm(totient, p_minus_1, q_minus_1);
    do {
        // generate random with nbits bits
        mpz_urandomb(e, state, nbits);
        gcd(denom, e, totient);
        // numbers will be coprime when GCD is 1
    } while (mpz_cmp_ui(denom, 1) != 0);
    mpz_clears(totient, p_minus_1, q_minus_1, denom, NULL);
    return;
}

// Writes the public key info to the given file
//
// n: the product of the primes
// e: the public exponent to raise to
// s: the signature of the key
// username: the username to write to file
// pbfile: the file to write to
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
    return;
}

// Reads the public key info from the given file
//
// n: the product of the primes to read to
// e: the public exponent to read to
// s: the signature of the key to read to
// username: the string to read the username to
// pbfile: the file to read from
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
    return;
}

// Creates the private key with the given
// primes and exponent, written to d
//
// d: the private key to write to
// e: the public exponent
// p: the first prime
// q: the second prime
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    // could use two variables, but three for readability
    mpz_t p_minus_1, q_minus_1, totient;
    mpz_inits(p_minus_1, q_minus_1, totient, NULL);
    // set p - 1 and q - 1
    mpz_sub_ui(p_minus_1, p, 1);
    mpz_sub_ui(q_minus_1, q, 1);
    lcm(totient, p_minus_1, q_minus_1);
    // d = e mod totient
    mod_inverse(d, e, totient);
    mpz_clears(p_minus_1, q_minus_1, totient, NULL);
    return;
}

// Writes the product of the primes and the
// private key to the given file
//
// n: the product of the primes for the key
// d: the private key
// pvfile: the file to write to
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
    return;
}

// Reads the private key from
// the given file
//
// n: the n value to set
// d: the private key toset
// pvfile: the file to read from
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);
    return;
}

// Encrypts the given number m with the public exponent and modulus
//
// c: the number to encrypt to
// m: the base of the exponent expression
// e: the public exponent
// n: the modulus
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

// Calculates the block size
// Returns: the block size k
//
// n: the modulus to find block size for
static size_t block_size(mpz_t n) {
    // block size is floor((log_2(n) - 1) / 8)
    return (mpz_sizeinbase(n, 2) - 1) / 8;
}

// Encrypts the contents of a file
// to outfile with the given modulus and exponent
//
// infile: the file to read bytes from
// outfile: the file to write the encrypted message to
// n: the modulus to use for encryption
// e: the public exponent to use for encryption
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    size_t k = block_size(n);
    size_t byte = sizeof(uint8_t);
    uint8_t *array = (uint8_t *) malloc(k * byte);
    array[0] = 0xFF;
    mpz_t m;
    mpz_init(m);
    uint64_t j = 0;
    // read into array, set j to number of bytes read, compare to k - 1
    // (if it's not k - 1 bytes, we have reached the end)
    while ((j = fread(array + 1, byte, k - 1, infile)) == k - 1) {
        // convert byte array to mpz_t
        mpz_import(m, j + 1, 1, byte, 1, 0, array);
        // encrypt the newly created mpz_t
        rsa_encrypt(m, m, e, n);
        // write to file
        gmp_fprintf(outfile, "%Zx\n", m);
    }
    // ensure leftover bytes are written as well
    mpz_import(m, j + 1, 1, byte, 1, 0, array);
    rsa_encrypt(m, m, e, n);
    gmp_fprintf(outfile, "%Zx\n", m);
    mpz_clear(m);
    free(array);
    return;
}

// Decrypts the encoded mpz_t by finding
// c^d mod n, and writes to m
//
// m: the value to write the decoded number to
// c: the encoded number
// d: the private key/exponent to raise to
// n: the modulus to take
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

// Decrypts an encrypted file with the given
// modulus and private key
//
// infile: the encrypted file to read from
// outfile: the file to write the decrypted data to
// n: the public modulus
// d: the private key to raise to
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    size_t k = block_size(n);
    size_t byte = sizeof(uint8_t);
    uint8_t *array = (uint8_t *) malloc(k * byte);
    mpz_t c;
    mpz_init(c);
    uint64_t j = 0;
    // read each line into c
    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
        // decrypt it
        rsa_decrypt(c, c, d, n);
        // convert it back to a byte array
        mpz_export(array, &j, 1, 1, byte, 0, c);
        // write the byte array to outfile
        fwrite(array + 1, byte, j - 1, outfile);
    }
    free(array);
    mpz_clear(c);
    return;
}

// Produces the message signature s by
// calculating m^d mod n
//
// s: the signature to output to
// m: the message to sign
// d: the private key to use as the exponent
// n: the public modulus
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

// Ensures the given signature matches
// the expected message
// Returns: whether or not the
// message matches the signature
//
// m: the expected message
// s: the signature to check against
// e: the public exponent
// n: the public modulus
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    // need to compare before clearing t, so we
    // store it in a variable
    bool verified = mpz_cmp(t, m) == 0;
    mpz_clear(t);
    return verified;
}
