#pragma once

// BSD 2-Clause License
//
// Copyright (c) 2021, Darrell Long
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t Set;

static const uint32_t mask = 0x00000003f; // Sets are numbered bits 0 through 63

static inline Set empty_set(void) {
    return 0;
}

static inline bool member_set(uint32_t x, Set s) {
    return (s & (0x1 << (x & mask)));
}

static inline Set insert_set(uint32_t x, Set s) {
    return (s | (0x1 << (x & mask)));
}

static inline Set delete_set(uint32_t x, Set s) {
    return (s & ~(0x1 << (x & mask)));
}

static inline Set union_set(Set s, Set t) {
    return (s | t);
}

static inline Set intersect_set(Set s, Set t) {
    return (s & t);
}

static inline Set difference_set(Set s, Set t) {
    return (s & ~t);
}

static inline Set complement_set(Set s) {
    return ~s;
}
