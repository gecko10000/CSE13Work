#pragma once

#include "node.h"
#include "code.h"
#include "defines.h"
#include <stdint.h>

Node *build_tree(uint64_t hist[static ALPHABET]);

void build_codes(Node *root, Code table[static ALPHABET]);

void dump_tree(int outfile, Node *root);

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]);

void delete_tree(Node **root);
