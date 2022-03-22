#include <stdlib.h>
#include <gmp.h>
#include "randstate.h"

gmp_randstate_t state;

// Initializes the related pseudorandom number generators
//
// seed: the seed to use for generation
void randstate_init(uint64_t seed) {
    srandom(seed);
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    return;
}

// Clears the state to ensure there are no memory leaks
void randstate_clear(void) {
    gmp_randclear(state);
    return;
}
