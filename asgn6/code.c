#include "code.h"
#include "defines.h"

#include <stdio.h>
#include <inttypes.h>

// Creates a new empty Code struct.
// Returns: the Code (by value!)
Code code_init(void) {
    Code code;
    code.top = 0;
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i++) {
        code.bits[i] = 0;
    }
    return code;
}

// Returns the size of the
// given Code.
//
// c: a pointer to the Code to check the size of
uint32_t code_size(Code *c) {
    return c->top;
}

// Returns whether or not
// the given Code is empty.
//
// c: a pointer to the Code to check
bool code_empty(Code *c) {
    return c->top == 0;
}

// Returns whether or not
// the given Code is full.
//
// c: a pointer to the Code to check
bool code_full(Code *c) {
    return c->top == ALPHABET;
}

// Returns whether or not the
// index is out of the range
// of a Code.
//
// i: the index to check
static bool out_of_range(uint32_t i) {
    return i >= ALPHABET;
}

// Sets the specified bit
// in the specified Code to 1.
// Returns: whether or not it was successfully set
//
// c: a pointer to the Code to set the bit in
// i: the index of the bit to set
bool code_set_bit(Code *c, uint32_t i) {
    if (out_of_range(i)) {
        return false;
    }
    // modify the i/8th byte, setting the ((i + 1) % 8)th bit to true
    c->bits[i / 8] |= (1 << i % 8);
    return true;
}

// Sets the specified bit
// in the specified Code to 0.
// Returns: whether or not it was successfully set
//
// c: a pointer to the Code to set the bit in
// i: the index of the bit to set
bool code_clr_bit(Code *c, uint32_t i) {
    if (out_of_range(i)) {
        return false;
    }
    // modify the i/8th byte, setting the ((i + 1) % 8)th bit to false
    c->bits[i / 8] &= ~(1 << i % 8);
    return true;
}

// Gets the bit set at the specified
// index in the specified Code.
// Returns: whether or not the bit is set to 0 or false if it's out of range
//
// c: a pointer to the Code to check
// i: the index of the bit to check
bool code_get_bit(Code *c, uint32_t i) {
    if (out_of_range(i)) {
        return false;
    }
    // isolates the ((i + 1) % 8)th bit in the i/8th byte and checks
    // whether the resulting number is nonzero
    return (c->bits[i / 8] & (1 << i % 8)) != 0;
}

// Sets the topmost bit to the given value
// and increments the top.
// Returns: whether or not the bit was pushed successfully
//
// c: a pointer to the Code to push the bit to
// bit: the value of the bit to set
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    if (bit == 0) {
        code_clr_bit(c, c->top++);
    } else {
        code_set_bit(c, c->top++);
    }
    return true;
}

// Gets the topmost bit in the code
// and decrements the top pointer.
// Returns: whether or not the bit was successfully retrieved
//
// c: a pointer to the Code to pop a bit from
// bit: a pointer to the bit to set the popped value to
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    *bit = code_get_bit(c, --c->top);
    return true;
}

// Prints the values of the Code,
// used for debugging.
//
// c: a pointer to the Code to print
void code_print(Code *c) {
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i++) {
        printf("%" PRIu8 " ", c->bits[i]);
    }
    printf("\n");
    return;
}
