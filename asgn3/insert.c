#include "stats.h"

// Sorts the given array using simple insertion, iterating backwards
// until the value is in the correct spot.
//
// stats: the struct to record moves and compares to
// A: the array to sort
// n: the size of the array
void insertion_sort(Stats *stats, uint32_t *A, uint32_t n) {
    reset(stats);
    for (uint32_t i = 1; i < n; i++) {
        uint32_t j = i;
        uint32_t temp = move(stats, A[i]);
        while (j > 0 && cmp(stats, A[j - 1], temp) == 1) {
            A[j] = move(stats, A[j - 1]);
            j--;
        }
        A[j] = move(stats, temp);
    }
    return;
}
