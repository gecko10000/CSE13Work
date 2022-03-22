#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "bv.h"

// copied from assignment
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

// Creates a new bit vector with the given length.
// Returns: a pointer to the bit vector.
//
// length: the size of the vector to create
BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    bv->length = length;
    bv->vector = (uint8_t *) calloc(length / 8 + (length % 8 == 0 ? 0 : 1), sizeof(uint8_t));
    if (bv->vector == NULL) {
        fprintf(stderr, "Could not allocate BitVector.\n");
        free(bv);
        return NULL;
    }
    return bv;
}

// Deletes the given bit vector.
//
// bv: a pointer to the address of the bit vector
void bv_delete(BitVector **bv) {
    free((*bv)->vector);
    free(*bv);
    *bv = NULL;
    return;
}

// Returns the length of the bit vector.
//
// bv: the vector to get the length of
uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

// A helper function for readability.
// Returns: whether or not the bit index is within the bounds of the vector.
//
// bv: the vector to compare against
// i: the index to check
static bool out_of_range(BitVector *bv, uint32_t i) {
    return i >= bv->length;
}

// Sets the specified bit in the vector to 1.
// Returns: whether the bit was set successfully.
//
// bv: the vector to set the bit in
// i: the index of the bit to set
bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (out_of_range(bv, i)) {
        return false;
    }
    bv->vector[i / 8] |= 1 << i % 8;
    return true;
}

// Sets the specified bit in the vector to 0.
// Returns: whether the bit was set successfully.
//
// bv: the vector to set the bit in
// i: the index of the bit to set
bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (out_of_range(bv, i)) {
        return false;
    }
    bv->vector[i / 8] &= ~(1 << i % 8);
    return true;
}

// Gets the state of the bit at the given index.
// Returns: the state of the bit, or false if the index is out of range.
//
// bv: the vector to get the bit from
// i: the index of the bit to get
bool bv_get_bit(BitVector *bv, uint32_t i) {
    if (out_of_range(bv, i)) {
        return false;
    }
    return (bv->vector[i / 8] >> i % 8) & 1; // isolate i%8th bit of i/8th element
}

// A debug function to print the contents of the bit vector.
//
// bv: the bit vector to print
void bv_print(BitVector *bv) {
    for (uint32_t i = 0; i < bv->length; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            printf("%d", (bv->vector[i] >> j) & 1);
        }
    }
    return;
}
