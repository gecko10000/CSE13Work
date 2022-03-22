#include "batcher.h"
#include "heap.h"
#include "insert.h"
#include "quick.h"
#include "set.h"
#include "stats.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN(X, Y)   (((X) < (Y)) ? (X) : (Y))
#define NUM_FORMAT  "%13" PRIu32
#define BIT_MASK    ((1 << 30) - 1)
#define FLAG_FORMAT "   -%c %-12s %-s\n"

// enum usage from https://www.tutorialspoint.com/enum-in-c and coding standards
typedef enum { insertion, batcher, heap, quick } SetBits;

// Shows usage of the program to the user,
// run when an invalid option is given or the -H flag is used
//
// arg0: the argument used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Generates pseudorandom numbers and sorts them with various algorithms.\n\n");

    printf("USAGE\n");
    printf("   %s [-Haibhq] [-n length] [-p elements] [-r seed]\n\n", arg0);

    printf("OPTIONS\n");
    printf(FLAG_FORMAT, 'H', "", "Display this message for help and usage.");
    printf(FLAG_FORMAT, 'a', "", "Enable all sorts.");
    printf(FLAG_FORMAT, 'i', "", "Enable Insertion Sort.");
    printf(FLAG_FORMAT, 'b', "", "Enable Batcher Sort.");
    printf(FLAG_FORMAT, 'h', "", "Enable Heap Sort.");
    printf(FLAG_FORMAT, 'q', "", "Enable Quick Sort.");
    printf(FLAG_FORMAT, 'n', "length", "Sets the array size (default 100)");
    printf(FLAG_FORMAT, 'p', "elements", "Sets the number of elements to print (default 100)");
    printf(FLAG_FORMAT, 'r', "seed", "Sets the seed (default 13371453)");
    exit(1);
    return;
}

// Translates the command-line option into
// bits in the set
// Returns: the set of options
//
// set: the set to add the option to
// c: the command-line option passed in
Set process_option(Set set, char c) {
    switch (c) {
    case 'a':
        set = insert_set(heap, set);
        set = insert_set(batcher, set);
        set = insert_set(insertion, set);
        set = insert_set(quick, set);
        return set;
    case 'h': return insert_set(heap, set);
    case 'b': return insert_set(batcher, set);
    case 'i': return insert_set(insertion, set);
    case 'q': return insert_set(quick, set);
    default: break;
    }
    return set;
}

// Populates the given array with randomly generated numbers
// Returns: a pointer to the first number in the array
//
// arr: the array to populate
// seed: the seed to generate random numbers from
// size: the size of the array
void generate_numbers(uint32_t *arr, uint32_t seed, uint32_t size) {
    srandom(seed);
    for (uint32_t i = 0; i < size; i++) {
        arr[i] = random() & BIT_MASK; // only take lower 30 bits
    }
    return;
}

// Prints the output of the sort
//
// sort_name: a char array of the sort name
// size: the size of the array
// stats: the stats struct to read values from
// to_print: the number of values to actually print out
// arr: the sorted numArray
void print_readably(
    char *sort_name, uint32_t size, Stats *stats, uint32_t to_print, uint32_t *arr) {
    printf("%s, %" PRIu32 " elements, %" PRIu64 " moves, %" PRIu64 " compares\n", sort_name, size,
        stats->moves, stats->compares);
    int nums = MIN(to_print, size);
    // calculate number of rows to print
    int rows = nums / 5 + (nums % 5 == 0 ? 0 : 1); // clunky ceiling function
    for (int i = 0; i < rows; i++) {
        // get the number of columns to print in the row,
        // which is 5 except for the last one, which can be less.
        int columns = (i + 1 == rows && nums % 5 != 0) ? nums % 5 : 5;
        for (int j = 0; j < columns; j++) {
            printf(NUM_FORMAT, arr[i * 5 + j]);
        }
        printf("\n"); // row done printing
    }
    return;
}

int main(int argc, char *argv[]) {
    //defaults
    uint32_t seed = 13371453;
    uint32_t size = 100;
    uint32_t to_print = 100;
    Set algorithm_set = empty_set();

    int option;
    while ((option = getopt(argc, argv, "ahbiqr:n:p:H")) != -1) {
        switch (option) {
        case 'a':
        case 'h':
        case 'b':
        case 'i':
        case 'q': algorithm_set = process_option(algorithm_set, option); break;
        case 'r': seed = strtoul(optarg, NULL, 10); break;
        case 'n': size = strtoul(optarg, NULL, 10); break;
        case 'p': to_print = strtoul(optarg, NULL, 10); break;
        case 'H':
        default: usage(argv[0]);
        }
    }

    if (algorithm_set == empty_set()) { // no sorts were specified
        printf("Select at least one sort to perform.\n");
        usage(argv[0]);
        return 1;
    }

    uint32_t *arr = (uint32_t *) malloc(size * sizeof(uint32_t)); // from coding guide
    Stats stats = { 0, 0 };
    void (*sorts[])(Stats *, uint32_t *, uint32_t)
        = { insertion_sort, batcher_sort, heap_sort, quick_sort };
    char *sort_names[] = { "Insertion Sort", "Batcher Sort", "Heap Sort", "Quick Sort" };
    for (int i = 0; i < 4; i++) {
        if (member_set(i,
                algorithm_set)) { // use enum values directly, maybe there's a better way, i don't know
            generate_numbers(arr, seed, size);
            sorts[i](&stats, arr, size); // call relative function
            print_readably(sort_names[i], size, &stats, to_print, arr);
        }
    }
    free(arr); // no memory leaks!
    return 0;
}
