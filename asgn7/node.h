#pragma once

#include <stdint.h>

typedef struct Node Node;

struct Node {
    char *word;
    uint32_t count;
};

Node *node_create(char *word);

void node_delete(Node **n);

void node_print(Node *n);
