#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "node.h"

// Creates a node with the given word.
// Returns: a pointer to the created node.
//
// word: the word to use for the node
Node *node_create(char *word) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->word = strdup(word);
    n->count = 0;
    return n;
}

// Deletes the given node.
//
// n: a pointer to the address of the node to delete
void node_delete(Node **n) {
    free((*n)->word);
    free(*n);
    *n = NULL;
    return;
}

// Debug function to print the node.
//
// n: the node to print
void node_print(Node *n) {
    if (n == NULL) {
        return;
    }
    printf("%s: %" PRIu32, n->word, n->count);
    return;
}
