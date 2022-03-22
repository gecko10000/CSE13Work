#include "stats.h"

// Adapted from given Python code

// Swaps the x index with the y index if
// the value at x > the value at y
//
// stats: the stats to increment
// A: the array with the values
// x: the index that should contain the low number
// y: the index that should contain the high number
static void comparator(Stats *stats, uint32_t *A, uint32_t x, uint32_t y) {
    if (cmp(stats, A[x], A[y]) == 1) {
        swap(stats, &(A[x]), &(A[y]));
    }
    return;
}

// Gets the number of bits required
// to store the given number
// Returns: the number of bits
//
// n: the number to calculate for
static int num_bits(uint32_t n) {
    int bits = 0;
    while (n > 0) {
        bits++;
        n /= 2;
    }
    return bits;
}

// Sorts batcherly.
// Sorts adjacent values with the comparator,
// then sorts two of those sorted pairs with
// each other, and so on in powers of two
// until the entire thing is sorted
//
// stats: the stats to increment
// A: the array to sort
// n: the size of the array
void batcher_sort(Stats *stats, uint32_t *A, uint32_t n) {
    reset(stats);
    if (n == 0) {
        return;
    }

    int t = num_bits(n);
    uint32_t p = 1 << (t - 1);

    while (p > 0) {
        uint32_t q = 1 << (t - 1);
        uint32_t r = 0;
        uint32_t d = p;

        while (d > 0) {
            for (uint32_t i = 0; i < n - d; i++) {
                if ((i & p) == r) {
                    comparator(stats, A, i, i + d);
                }
            }
            d = q - p;
            q >>= 1;
            r = p;
        }
        p >>= 1;
    }
    return;
}
