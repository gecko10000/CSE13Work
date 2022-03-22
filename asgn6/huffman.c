#include "huffman.h"
#include "defines.h"
#include "pq.h"
#include "io.h"
#include "node.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

// Creates a Huffman tree from
// the given byte histogram.
// Returns: a pointer to the root node of the tree
//
// hist: the array of byte occurrences
Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *q = pq_create(ALPHABET);
    for (int i = 0; i < ALPHABET; i++) {
        if (hist[i] == 0) {
            continue;
        }
        enqueue(q, node_create(i, hist[i]));
    }
    while (pq_size(q) > 1) {
        Node *n1, *n2;
        dequeue(q, &n1);
        dequeue(q, &n2);
        enqueue(q, node_join(n1, n2));
    }
    Node *n;
    dequeue(q, &n);
    pq_delete(&q);
    return n;
}

// Creates Code sequences for each symbol in the tree.
// Inspired by the pseudocode on page 15
//
// root: a pointer to the root node representing the tree
// table: the array to fill with the created Codes
void build_codes(Node *root, Code table[static ALPHABET]) {
    static Code c = {
        .top = 0, .bits = { 0 }
    }; // inspired by ben in Discord https://discord.com/channels/926211705766305842/926211706382868584/946617342052429865
    static uint8_t ignored;
    if (root == NULL) {
        return;
    }
    if (root->left == NULL && root->right == NULL) {
        table[root->symbol] = c;
    } else {
        code_push_bit(&c, 0);
        build_codes(root->left, table);
        code_pop_bit(&c, &ignored);

        code_push_bit(&c, 1);
        build_codes(root->right, table);
        code_pop_bit(&c, &ignored);
    }
    return;
}

// Writes the buffer if it's full (or if flush is true).
//
// outfile: the file descriptor of the file to write to
// buf: the array of bytes to write
// bytes: a pointer to the number of bytes in the array
// flush: a boolean to override the full array check, useful when flushing
static void write_if_ready(int outfile, uint8_t *buf, int *bytes, bool flush) {
    if (*bytes < BLOCK && !flush) {
        return;
    }
    write_bytes(outfile, buf, *bytes);
    *bytes = 0;
    return;
}

static uint8_t buf[BLOCK];

// Writes the tree to the file, utilizing a buffer
// to decrease the number of write operations.
//
// outfile: the file descriptor of the file to write to
// root: a pointer to the root node of the tree
void dump_tree(int outfile, Node *root) {
    int bytes = 0;
    // base case
    if (root == NULL) {
        return;
    }
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);
    if (root->left == NULL && root->right == NULL) {
        // leaf node
        buf[bytes++] = 'L';
        write_if_ready(outfile, buf, &bytes, false);
        buf[bytes++] = root->symbol;
        write_if_ready(outfile, buf, &bytes, false);
    } else {
        // has child(ren)
        buf[bytes++] = 'I';
        write_if_ready(outfile, buf, &bytes, false);
    }
    // flush remaining bytes
    write_if_ready(outfile, buf, &bytes, true);
    return;
}

// Reconstructs the Huffman tree with
// an array of bytes representing the encoded tree.
// Returns: a pointer to the root node of the tree
//
// nbytes: the size of the array/encoded tree
// tree: the array of bytes to read from
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *stack = stack_create(nbytes);
    uint16_t i = 0;
    while (i < nbytes) {
        if (tree[i] == 'L') {
            stack_push(stack, node_create(tree[++i], 1));
        } else if (tree[i] == 'I') {
            Node *right, *left;
            stack_pop(stack, &right);
            stack_pop(stack, &left);
            stack_push(stack, node_join(left, right));
        }
        i++;
    }
    Node *root;
    stack_pop(stack, &root);
    stack_delete(&stack);
    return root;
}

// Recursively deletes the given tree
// using post traversal order.
//
// root: a pointer to the address of a node (double pointer because we need to set the address to NULL after deleting
void delete_tree(Node **root) {
    if (root == NULL || *root == NULL) {
        return;
    }
    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);
    node_delete(root);
    root = NULL;
    return;
}
