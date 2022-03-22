#include "universe.h"

#include <inttypes.h>
#include <stdlib.h>

struct Universe {
    uint32_t rows;
    uint32_t cols;
    bool **grid;
    bool toroidal;
};

// Creates a Universe with the specified size and toroidality
// Returns: a pointer to the struct pointer
//
// rows: the number of rows for the grid
// cols: the number of columns for the grid
// toroidal: whether or not to make the universe toroidal
Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    // following example from assignment for allocating ints
    bool **grid = (bool **) calloc(rows, sizeof(bool *));
    for (uint32_t r = 0; r < rows; r++) {
        grid[r] = (bool *) calloc(cols, sizeof(bool));
    }
    Universe *u = malloc(sizeof(Universe));
    u->rows = rows;
    u->cols = cols;
    u->grid = grid;
    u->toroidal = toroidal;
    return u;
}

// Frees all allocated objects in the
// universe struct, including the universe itself
//
// u: the universe to delete
void uv_delete(Universe *u) {
    for (uint32_t r = 0; r < u->rows; r++) {
        free(u->grid[r]);
        u->grid[r] = NULL;
    }
    free(u->grid);
    u->grid = NULL;
    free(u);
    u = NULL;
    return;
}

// Gets the number of rows in the universe
// Returns the number of rows
//
// u: the universe to get the number of rows for
uint32_t uv_rows(Universe *u) {
    return u->rows;
}

// Gets the number of columns in the universe
// Returns the number of columns
//
// u: the universe to get the number of columns for
uint32_t uv_cols(Universe *u) {
    return u->cols;
}

// Check whether i is between [0, upper)
// Returns: whether or not it is
//
// i: the number to check
// upper: the upper bound of the range
static bool in_bounds(int64_t i, int64_t upper) {
    return i >= 0 && i < upper;
}

// Checks whether or not the specified position
// is within bounds of the universe
// Returns true if it's in bounds, false otherwise
//
// u: the universe to compare against
// r: the row of the coordinate to check
// c: the column of the coordinate to check
static bool verify(Universe *u, int64_t r, int64_t c) {
    return in_bounds(r, u->rows) && in_bounds(c, u->cols);
}

// Sets the specified cell to be alive
//
// u: the universe to set the cell state in
// r: the row of the coordinate to set
// c: the column of the coordinate to set
void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {
    if (!verify(u, r, c)) {
        return;
    }
    u->grid[r][c] = true;
    return;
}

// Sets the specified cell to be dead
//
// u: the universe to set the cell state in
// r: the row of the coordinate to set
// c: the column of the coordinate to set
void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    if (!verify(u, r, c)) {
        return;
    }
    u->grid[r][c] = false;
    return;
}

// Gets whether or not the cell at the
// coordinates is alive
// Returns true if the cell is alive and the
// coordinates are within the bounds of the universe
//
// u: the universe to check
// r: the row to retrieve from
// c: the column to retrieve from
bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    return verify(u, r, c) && u->grid[r][c];
}

// Fills the universe's grid with live
// cells, reading from the input file
// Returns whether or not the universe was successfully populated
//
// u: the universe to populate
// infile: the file to read from
bool uv_populate(Universe *u, FILE *infile) {
    uint32_t row, col;
    while (fscanf(infile, "%" PRIu32 "%" PRIu32, &row, &col) != EOF) {
        if (row >= u->rows || col >= u->cols) {
            return false;
        }
        uv_live_cell(u, row, col);
    }
    return true;
}

// Provides modulus for negative numbers by adding
// the divisor to them (i.e. -1 % 5 = -1+5 % 5 = 4),
// essentially normalizing the range [0, b).
// Returns: the normalized number
//
// a: the number to normalize
// b: the upper bound of the range
static uint32_t normalize(int64_t a, int64_t b) {
    return (uint32_t)((a + b) % b);
}

// Checks the states of all of a specified cell's
// neighbors, taking toroidality into account
// Returns the number of live neighbors
//
// u: the universe to use
// r: the row of the cell to center around
// c: the column of the cell to center around
uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    uint32_t live_adjacent = 0;
    // using int64_t because it needs to be able to include -1 and UINT_MAX+1
    for (int64_t row = ((int64_t) r) - 1; row <= ((int64_t) r) + 1; row++) {
        for (int64_t col = ((int64_t) c) - 1; col <= ((int64_t) c) + 1; col++) {
            if (row == r && col == c) { // skip self
                continue;
            }
            if (!u->toroidal && !verify(u, row, col)) { // skip out of bounds when not toroidal
                continue;
            }
            live_adjacent += uv_get_cell(u, normalize(row, u->rows), normalize(col, u->cols));
        }
    }
    return live_adjacent;
}

// Prints the universe's cell states
//
// u: the universe to print
// outfile: the file to print to
void uv_print(Universe *u, FILE *outfile) {
    for (uint32_t r = 0; r < u->rows; r++) {
        for (uint32_t c = 0; c < u->cols; c++) {
            fputc(u->grid[r][c] ? 'o' : '.',
                outfile); // print live cells as 'o' and dead cells as '.'
        }
        fputc('\n', outfile);
    }
    return;
}
