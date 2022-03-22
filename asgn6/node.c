#include "node.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

// Creates a node with the specified
// symbol and frequency.
// Returns: a pointer to the created Node
//
// symbol: the byte symbol for the Node
// frequency: the amount to put into the Node
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Deletes the given node.
//
// n: a pointer to the address of the Node (double pointer because we want to set it to NULL afterwards)
void node_delete(Node **n) {
    free(*n);
    n = NULL;
    return;
}

// Joins two nodes together into
// one with a left and a right child.
// Returns: a pointer to the newly created pointer
//
// left: a pointer to the Node to set as the left one
// right: a pointer to the Node to set as the right one
Node *node_join(Node *left, Node *right) {
    Node *joined = node_create('$', left->frequency + right->frequency);
    joined->left = left;
    joined->right = right;
    return joined;
}

// Prints the given Node (for debugging).
//
// n: a pointer to the Node to print
void node_print(Node *n) {
    printf("%c: %" PRIu64 "\n", n->symbol, n->frequency);
    if (n->left != NULL) {
        printf("left: ");
        node_print(n->left);
    }
    if (n->right != NULL) {
        printf("right: ");
        node_print(n->right);
    }
    return;
}
