#include "stats.h"

// Adapted from given Python code

// Gets whichever child is larger
// Returns: the index of the larger child
//
// stats: the stats to increment
// A: the heap array
// first: the parent index
// last: the size of the array
static uint32_t max_child(Stats *stats, uint32_t *A, uint32_t first, uint32_t last) {
    uint32_t left = 2 * first;
    uint32_t right = left + 1;
    if (right <= last && cmp(stats, A[right - 1], A[left - 1]) == 1) {
        return right;
    }
    return left;
}

// Moves items up the heap if they are found
// to be in the wrong order
//
// stats: the stats to increment
// A: the heap array
// first: the parent index
// last: the size of the array
static void fix_heap(Stats *stats, uint32_t *A, uint32_t first, uint32_t last) {
    uint32_t mother = first;
    uint32_t great = max_child(stats, A, mother, last);

    while (mother <= last / 2) {
        if (cmp(stats, A[great - 1], A[mother - 1]) != 1) {
            break;
        }
        swap(stats, &(A[mother - 1]), &(A[great - 1]));
        mother = great;
        great = max_child(stats, A, mother, last);
    }
    return;
}

// Goes through the bottom layer of the heap
// and calls fix_heap on each one
//
// stats: the stats to increment
// A: the heap array
// first: the parent index
// last: the size of the array
static void build_heap(Stats *stats, uint32_t *A, uint32_t first, uint32_t last) {
    for (uint32_t father = last / 2; father > first - 1; father--) {
        fix_heap(stats, A, father, last);
    }
    return;
}

// Sorts heapingly.
// Builds and sorts the heap, then
// turns the ordered heap back into
// an ordered array
//
// stats: the stats to increment
// A: the array to sort
// n: the size of the array
void heap_sort(Stats *stats, uint32_t *A, uint32_t n) {
    reset(stats);
    uint32_t first = 1;
    uint32_t last = n;
    build_heap(stats, A, first, last);
    for (uint32_t leaf = last; leaf > first; leaf--) {
        swap(stats, &(A[first - 1]), &(A[leaf - 1]));
        fix_heap(stats, A, first, leaf - 1);
    }
    return;
}
