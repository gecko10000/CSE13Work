#include "universe.h"

#include <inttypes.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DELAY       50000
#define FLAG_FORMAT "   -%c %-12s %-s\n"

// Shows program usage to the user
//
// arg0: the argument used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Creates a universe with live and dead cells, then runs the Game of Life for a "
           "specified number of iterations.\n\n");

    printf("USAGE\n");
    printf("   %s [-Hts] [-i input] [-o output] [-n generations]\n\n", arg0);

    printf("OPTIONS\n");
    printf(FLAG_FORMAT, 'H', "", "Display this message for help and usage.");
    printf(FLAG_FORMAT, 't', "", "Set the universe to be toroidal.");
    printf(FLAG_FORMAT, 's', "", "Set the simulation to run silently.");
    printf(FLAG_FORMAT, 'i', "file", "Sets the file to use for input (default stdin).");
    printf(FLAG_FORMAT, 'o', "file", "Sets the file to use for output (default stdout).");
    printf(FLAG_FORMAT, 'n', "generations", "Sets the number of generations to simulate.");
    exit(1);
    return;
}

// Prints all live cells at their
// coordinates using ncurses
//
// u: the universe to print
void ncurses_print(Universe *u) {
    clear();
    for (uint32_t r = 0; r < uv_rows(u); r++) {
        for (uint32_t c = 0; c < uv_cols(u); c++) {
            if (uv_get_cell(u, r, c)) {
                mvprintw(r, c, "o");
            }
        }
    }
    refresh();
    return;
}

// Steps through one generation of the
// Game of Life
//
// from_u: the universe to compare against
// to_u: the universe to set the new cell states in
void advance(Universe *from_u, Universe *to_u) {
    for (uint32_t r = 0; r < uv_rows(from_u); r++) {
        for (uint32_t c = 0; c < uv_cols(from_u); c++) {
            bool alive = uv_get_cell(from_u, r, c);
            uint32_t live_neighbors = uv_census(from_u, r, c);
            // live with 2/3 neighbors
            if (alive && (live_neighbors == 2 || live_neighbors == 3)) {
                uv_live_cell(to_u, r, c);
                continue;
            }
            // dead and 3 live
            if (!alive && live_neighbors == 3) {
                uv_live_cell(to_u, r, c);
                continue;
            }
            // all others die
            uv_dead_cell(to_u, r, c);
        }
    }
    return;
}

int main(int argc, char *argv[]) {
    // default options
    bool toroidal = false;
    bool silent = false;
    uint32_t generations = 100;
    FILE *input = stdin;
    FILE *output = stdout;

    int option;
    while ((option = getopt(argc, argv, "tsn:i:o:H")) != -1) {
        switch (option) {
        case 't': toroidal = true; break;
        case 's': silent = true; break;
        case 'n': generations = strtoul(optarg, NULL, 10); break;
        case 'i': input = fopen(optarg, "r"); break;
        case 'o': output = fopen(optarg, "w"); break;
        case 'H':
        default: usage(argv[0]);
        }
    }
    int rows;
    int cols;
    fscanf(input, "%d %d", &rows, &cols); // read first line of file
    Universe *u_a = uv_create(rows, cols, toroidal), *u_b = uv_create(rows, cols, toroidal);
    uv_populate(u_a, input); // populate u_a with rest of file
    fclose(input);
    if (!silent) {
        initscr();
        curs_set(false);
    }
    for (uint32_t i = 0; i < generations; i++) {
        if (!silent) {
            ncurses_print(u_a);
            usleep(DELAY);
        }
        advance(u_a, u_b);
        Universe *temp = u_b; // just a pointer, don't need to free
        u_b = u_a;
        u_a = temp;
    }
    if (!silent) {
        endwin();
    }
    uv_print(u_a, output);
    fclose(output);
    uv_delete(u_a);
    uv_delete(u_b);
    return 0;
}
