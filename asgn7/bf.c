#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "bf.h"
#include "bv.h"
#include "salts.h"
#include "speck.h"

// copied from assignment
struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

// Creates a new bloom filter with the specified size.
// Returns: a pointer to the bloom filter.
//
// size: the size of the bit vector (and therefore the filter)
BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;
    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;
    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;
    bf->filter = bv_create(size);
    if (bf->filter == NULL) {
        free(bf);
        return NULL;
    }
    return bf;
}

// Deletes the given bloom filter.
//
// bf: a pointer to the address of the bloom filter
void bf_delete(BloomFilter **bf) {
    bv_delete(&(*bf)->filter);
    free(*bf);
    *bf = NULL;
    return;
}

// Returns the size of the give bloom filter.
//
// bf: a pointer to the filter to get the size of
uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

// A helper function to calculate the hashes
// of a given word with the three salts in the bloom filter.
//
// bf: the filter to use for the salts
// word: the word to hash
// h1: the location to store the first hash in
// h2: the location to store the second hash in
// h3: the location to store the third hash in
static void calc_hashes(BloomFilter *bf, char *word, uint32_t *h1, uint32_t *h2, uint32_t *h3) {
    *h1 = hash(bf->primary, word) % bv_length(bf->filter);
    *h2 = hash(bf->secondary, word) % bv_length(bf->filter);
    *h3 = hash(bf->tertiary, word) % bv_length(bf->filter);
    return;
}

// Inserts the word into the bloom filter.
//
// bf: the filter to insert into
// word: the word to hash and insert into the filter
void bf_insert(BloomFilter *bf, char *word) {
    uint32_t h1, h2, h3;
    calc_hashes(bf, word, &h1, &h2, &h3);
    BitVector *bv = bf->filter;
    bv_set_bit(bv, h1);
    bv_set_bit(bv, h2);
    bv_set_bit(bv, h3);
    return;
}

// Probes the bloom filter for the given word.
// Returns: whether or not the word is in the filter.
// When returning false, this function is correct.
// There may, however, be some false positives.
//
// bf: the filter to probe
// word: the word to hash and probe for
bool bf_probe(BloomFilter *bf, char *word) {
    uint32_t h1, h2, h3;
    calc_hashes(bf, word, &h1, &h2, &h3);
    BitVector *bv = bf->filter;
    return bv_get_bit(bv, h1) && bv_get_bit(bv, h2) && bv_get_bit(bv, h3);
}

// Debug function to print the bloom filter.
//
// bf: the filter to print
void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
    return;
}
