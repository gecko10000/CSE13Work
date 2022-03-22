#include "numtheory.h"
#include "randstate.h"

// Calculates the greatest common divisor
// and puts it into d
// Adapted from pseudocode on assignment
//
// d: the mpz_t variable to assign the GCD to
// a: the first number for the GCD algorithm
// b: the second number for the algorithm
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t ac, bc; // create copies so as to not modify original values
    mpz_init_set(ac, a);
    mpz_init_set(bc, b);
    while (mpz_sgn(bc) != 0) { // mpz_sgn returns 1 if positive, 0 if 0
        mpz_t t;
        mpz_init_set(t, bc); // set temp variable to b
        mpz_mod(bc, ac, bc); // set the mod to b
        mpz_set(ac, t); // set a to b's starting value
        mpz_clear(t);
    }
    mpz_set(d, ac); // set the GCD (a) to d
    mpz_clears(ac, bc, NULL);
    return;
}

// Calculates the modular inverse of a mod n
// where (a * i) mod n == 1
//
// i: the value to set the output to (the inverse, or 0 if no inverse found)
// a: the number to find the inverse of
// n: the modulus to calculate for
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r, rp, t, tp;
    mpz_init_set(r, n);
    mpz_init_set(rp, a);
    mpz_init_set_ui(t, 0);
    mpz_init_set_ui(tp, 1);
    mpz_t q, temp;
    mpz_inits(q, temp, NULL);
    while (mpz_sgn(rp) != 0) {
        mpz_tdiv_q(q, r, rp);
        // (r, r') <- (r', r - q * r')
        mpz_mul(temp, q, rp);
        mpz_sub(r, r, temp);
        // now we simplified to (r, r') <- (r', r)
        // and we can just swap them
        mpz_swap(r, rp);

        // (t, t') <- (t', t - q * t')
        mpz_mul(temp, q, tp);
        mpz_sub(t, t, temp);
        // (t, t') <- (t', t)
        mpz_swap(t, tp);
    }
    if (mpz_cmp_ui(r, 1) > 0) { // r > 1
        mpz_set_ui(i, 0); // 0 is equivalent of no inverse in this context
    } else {
        if (mpz_cmp_si(t, 0) < 0) { // t < 0
            mpz_add(t, t, n);
        }
        mpz_set(i, t);
    }
    mpz_clears(q, temp, r, rp, t, tp, NULL);
    return;
}

// Efficiently calculates the remainder of
// base raised to exponent divided by modulus,
// storing it in the out variable
//
// out: the variable to store the output to
// base: the base of the input
// exponent: the exponent of the input
// modulus: the divisor to use for mod calculations
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t v, p, dc, temp;
    mpz_init_set_ui(v, 1);
    mpz_init_set(p, base);
    mpz_init_set(dc, exponent);
    mpz_init(temp);
    while (mpz_sgn(dc) > 0) { // loop until dc is 0
        // check if odd
        mpz_mod_ui(temp, dc, 2);
        if (mpz_cmp_ui(temp, 1) == 0) { // remainder is 1, therefore dc is odd
            // v <- (v * p) mod n
            mpz_mul(temp, v, p);
            mpz_mod(v, temp, modulus);
        }
        // p <- (p * p) mod n
        mpz_mul(temp, p, p);
        mpz_mod(p, temp, modulus);
        // halve d
        mpz_fdiv_q_ui(dc, dc, 2);
    }
    mpz_set(out, v);
    mpz_clears(v, p, dc, temp, NULL);
    return;
}

// Gets the number of times n can be
// divided by 2 evenly and assigns it
// to o
//
// o: the output
// n: the value to calculate for
static void whole_2_divs_and_odd_factor(mpz_t q, mpz_t r, mpz_t n) {
    mpz_t nc, temp;
    mpz_init_set(nc, n);
    mpz_init(temp);
    mpz_mod_ui(temp, nc, 2);
    while (mpz_cmp_ui(temp, 0) == 0) { // while it's even
        mpz_add_ui(q, q, 1);
        mpz_fdiv_q_ui(nc, nc, 2);
        mpz_mod_ui(temp, nc, 2); // take mod again for while loop
    }
    mpz_set(r, nc);
    mpz_clears(nc, temp, NULL);
    return;
}

// Efficiently checks whether a
// number is prime with the given number
// of iterations.
// Returns: whether or not n is prime
//
// n: the number to check
// iters: the number of times to check
// (not 100% accurate but many iterations will
// reduce the chance of a false positive to almost 0)
bool is_prime(mpz_t n, uint64_t iters) {
    mpz_t n_minus_1, s, r, a, y, two, j, n_minus_3;
    mpz_inits(n_minus_1, s, r, a, y, j, n_minus_3, NULL);
    mpz_init_set_ui(two, 2); // set constants
    mpz_sub_ui(n_minus_1, n, 1);
    mpz_sub_ui(n_minus_3, n, 3);
    whole_2_divs_and_odd_factor(s, r, n_minus_1); // assign s and r in (n - 1 = 2^s * r)
    bool prime = true; // set to true by default, try to disprove
    // pick a random number and test for the number of specified tries
    for (uint64_t i = 1; i <= iters && prime; i++) {
        mpz_urandomm(a, state, n_minus_3); // gets a random number from [0, n-3)
        mpz_add_ui(a, a, 2); // changes range to [2, n-1)
        pow_mod(y, a, r, n);
        if (mpz_cmp_ui(y, 1) == 0 || mpz_cmp(y, n_minus_1) == 0) {
            // opposite case and early continue for readability
            continue;
        }
        mpz_set_ui(j, 1);
        while (mpz_cmp(j, s) < 0 && mpz_cmp(y, n_minus_1) != 0) {
            pow_mod(y, y, two, n);
            if (mpz_cmp_ui(y, 1) == 0) {
                prime = false;
                break;
            }
            mpz_add_ui(j, j, 1);
        }
        if (mpz_cmp(y, n_minus_1) != 0) {
            prime = false;
        }
    }
    mpz_clears(n_minus_1, s, r, a, y, two, j, n_minus_3, NULL);
    return prime;
}

// Creates a prime number p with at least
// <bits> bits.
//
// p: the output variable for the prime number
// bits: the minimum number of bits to have in the prime number
// iters: the number of times to test primality
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t extra;
    mpz_init(extra);
    // ensures p is always at least 2^bits
    mpz_ui_pow_ui(extra, 2, bits);
    do {
        mpz_urandomb(p, state, bits);
        mpz_add(p, p, extra);
    } while (!is_prime(p, iters));
    mpz_clear(extra);
    return;
}
