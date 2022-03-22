#include "pq.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

struct PriorityQueue {
    uint32_t size;
    uint32_t min_size;
    Node **values;
};

// Creates a new PriorityQueue with
// the specified the capacity.
// Returns: a pointer to the PriorityQueue
//
// capacity: the size of the PriorityQueue
PriorityQueue *pq_create(uint32_t capacity) {
    Node **values = (Node **) malloc(capacity * sizeof(Node *));
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    pq->min_size = capacity;
    pq->values = values;
    return pq;
}

// Deletes the given PriorityQueue.
//
// q: a pointer to the address of the PriorityQueue (double pointer to set to NULL)
void pq_delete(PriorityQueue **q) {
    free((*q)->values);
    free(*q);
    q = NULL;
    return;
}

// Returns whether or not the PriorityQueue is empty.
//
// q: a pointer to the PriorityQueue to check
bool pq_empty(PriorityQueue *q) {
    return q->size == 0;
}

// Returns whether or not the PriorityQueue is full.
//
// q: a pointer to the PriorityQueue to check
bool pq_full(PriorityQueue *q) {
    return q->size == q->min_size;
}

// Returns the size of the PriorityQueue.
//
// q: a pointer to the PriorityQueue to get the size of
uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

// Gets whichever child is smaller.
// Modified from assignment 3
// Returns: the index of the smaller child
//
// values: the heap array of node pointers
// first: the parent index
// last: the size of the array
static uint32_t min_child(Node **values, uint32_t first, uint32_t last) {
    uint32_t left = 2 * first;
    uint32_t right = left + 1;
    if (right <= last && values[right - 1]->frequency < values[left - 1]->frequency) {
        return right;
    }
    return left;
}

// Moves items up the heap if they
// are in the wrong order.
// Modified from assignment 3
//
// values: the heap array of node pointers to fix
// first: the parent index
// last: the size of the array
static void fix_heap(Node **values, uint32_t first, uint32_t last) {
    uint32_t mother = first;
    uint32_t min = min_child(values, mother, last);

    while (mother <= last / 2) {
        if (values[min - 1]->frequency >= values[mother - 1]->frequency) {
            break;
        }
        Node *temp = values[min - 1];
        values[min - 1] = values[mother - 1];
        values[mother - 1] = temp;
        mother = min;
        min = min_child(values, mother, last);
    }
    return;
}

// Goes through the bottom layer of the heap
// and calls fix_heap on each one.
// Modified from assignment 3
//
// values: the heap array of node pointers
// first: the parent index
// last: the size of the array
static void build_heap(Node **values, uint32_t first, uint32_t last) {
    for (uint32_t father = last / 2; father > first - 1; father--) {
        fix_heap(values, father, last);
    }
    return;
}

// Adds the Node into the PriorityQueue
// and immediately builds the heap.
// Returns: whether or not the Node was enqueued
//
// q: a pointer to the PriorityQueue to add to
// n: the Node pointer to add to the queue
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q)) {
        return false;
    }
    q->values[(q->size)++] = n;
    build_heap(q->values, 1, q->size);
    return true;
}

// Removes the lowest-frequency Node pointer
// from the PriorityQueue and puts it into
// the double pointer.
// Returns: whether or not the Node pointer was successfully dequeued
//
// q: a pointer to the PriorityQueue to add to
// n: a pointer to the address of a Node to put the dequeued value into
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }
    *n = q->values[0];
    q->values[0] = q->values[--(q->size)];
    build_heap(q->values, 1, q->size);
    return true;
}

// Prints the PriorityQueue for debugging.
//
// q: a pointer to the PriorityQueue to print
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size; i++, printf("->")) {
        printf("%c|%" PRIu64, q->values[i]->symbol, q->values[i]->frequency);
    }
    printf("\n");
    return;
}
