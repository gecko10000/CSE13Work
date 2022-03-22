#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct BitVector BitVector;

BitVector *bv_create(uint32_t length);

void bv_delete(BitVector **bv);

uint32_t bv_length(BitVector *bv);

bool bv_set_bit(BitVector *bv, uint32_t i);

bool bv_clr_bit(BitVector *bv, uint32_t i);

bool bv_get_bit(BitVector *bv, uint32_t i);

void bv_print(BitVector *bv);
