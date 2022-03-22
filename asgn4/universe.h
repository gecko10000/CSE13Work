#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct Universe Universe;

Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal);

void uv_delete(Universe *u);

uint32_t uv_rows(Universe *u);

uint32_t uv_cols(Universe *u);

void uv_live_cell(Universe *u, uint32_t r, uint32_t c);

void uv_dead_cell(Universe *u, uint32_t r, uint32_t c);

bool uv_get_cell(Universe *u, uint32_t r, uint32_t c);

bool uv_populate(Universe *u, FILE *infile);

uint32_t uv_census(Universe *u, uint32_t r, uint32_t c);

void uv_print(Universe *u, FILE *outfile);
