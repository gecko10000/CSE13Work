#include "stats.h"

// Adapted from Python code

// Loops through the given subarray and
// moves lower numbers to the start of the
// subarray, then moves the center to the back.
// Returns: the midpoint of the subarray
//
// stats: the stats to increment
// A: the array to sort
// lo: the lowest index of the subarray
// hi: the highest index of the subarray
static uint32_t partition(Stats *stats, uint32_t *A, uint32_t lo, uint32_t hi) {
    uint32_t i = lo - 1;
    for (uint32_t j = lo; j < hi; j++) {
        // move numbers lower than the separator below it,
        // higher ones above it
        if (cmp(stats, A[hi - 1], A[j - 1]) == 1) {
            i++;
            swap(stats, &(A[i - 1]), &(A[j - 1]));
        }
    }
    // move the midpoint to the end of the subarray
    swap(stats, &(A[i]), &(A[hi - 1]));
    return ++i;
}

// A recursive function to call partition
// on the array, then call itself on each
// half of the array until the partitions are size 0.
//
// stats: the stats to increment
// A: the array to sort
// lo: the low index of the subarray
// hi: the high index of the subarray
static void quick_sorter(Stats *stats, uint32_t *A, uint32_t lo, uint32_t hi) {
    if (lo >= hi) {
        return;
    }
    uint32_t p = partition(stats, A, lo, hi);
    quick_sorter(stats, A, lo, p - 1);
    quick_sorter(stats, A, p + 1, hi);
    return;
}

// Sorts quickly™
// Recursively calls quick_sorter, which
// sorts proceedingly smaller partitions of the array.
//
// stats: the stats to increment
// A: the array to sort
// n: the size of the array
void quick_sort(Stats *stats, uint32_t *A, uint32_t n) {

    reset(stats);
    quick_sorter(stats, A, 1, n);
    return;
}
