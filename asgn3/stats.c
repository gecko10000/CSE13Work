#include "stats.h"

int cmp(Stats *stats, uint32_t x, uint32_t y) {
    stats->compares += 1;
    if (x < y) {
        return -1;
    } else if (x > y) {
        return 1;
    } else {
        return 0;
    }
}

uint32_t move(Stats *stats, uint32_t x) {
    stats->moves += 1;
    return x;
}

void swap(Stats *stats, uint32_t *x, uint32_t *y) {
    stats->moves += 3;
    uint32_t t = *x;
    *x = *y;
    *y = t;
}

void reset(Stats *stats) {
    stats->moves = stats->compares = 0;
}
