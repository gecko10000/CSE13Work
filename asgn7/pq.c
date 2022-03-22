#include <stdlib.h>
#include <stdio.h>

#include "pq.h"

typedef struct Entry {
    char *name;
    double dist;
} Entry;

struct PriorityQueue {
    uint32_t size;
    uint32_t capacity;
    Entry **entries;
};

// Creates a priority queue with the specified capacity.
// Returns: a pointer to the created queue
//
// capacity: the size of the queue
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (pq == NULL) {
        return NULL;
    }
    pq->size = 0;
    pq->capacity = capacity;
    pq->entries = (Entry **) calloc(capacity, sizeof(Entry *));
    if (pq->entries == NULL) {
        free(pq);
        return NULL;
    }
    return pq;
}

// Deletes the given queue.
//
// q: a pointer to the address of the queue to delete
void pq_delete(PriorityQueue **q) {
    for (uint32_t i = 0; i < (*q)->size; i++) {
        free((*q)->entries[i]->name);
        free((*q)->entries[i]);
    }
    free((*q)->entries);
    free(*q);
    *q = NULL;
    return;
}

// Returns whether or not the queue is empty.
//
// q: the queue to check
bool pq_empty(PriorityQueue *q) {
    return q->size == 0;
}

// Returns whether or not the queue is full.
//
// q: the queue to check
bool pq_full(PriorityQueue *q) {
    return q->size == q->capacity;
}

// Returns the size of the priority queue.
//
// q: the queue to get the size of
uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

// Gets whichever child is smaller.
// Modified from assignment 3
// Returns: the index of the smaller child
//
// values: the heap array of entries
// first: the parent index
// last: the size of the array
static uint32_t min_child(Entry **values, uint32_t first, uint32_t last) {
    uint32_t left = 2 * first;
    uint32_t right = left + 1;
    if (right <= last && values[right - 1]->dist < values[left - 1]->dist) {
        return right;
    }
    return left;
}

// Moves items up the heap if they
// are in the wrong order.
// Modified from assignment 3
//
// values: the heap array of entries to fix
// first: the parent index
// last: the size of the array
static void fix_heap(Entry **values, uint32_t first, uint32_t last) {
    uint32_t mother = first;
    uint32_t min = min_child(values, mother, last);

    while (mother <= last / 2) {
        if (values[min - 1]->dist >= values[mother - 1]->dist) {
            break;
        }
        Entry *temp = values[min - 1];
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
// values: the heap array of entries
// first: the parent index
// last: the size of the array
static void build_heap(Entry **values, uint32_t first, uint32_t last) {
    for (uint32_t father = last / 2; father > first - 1; father--) {
        fix_heap(values, father, last);
    }
    return;
}

// Adds the name and distance to the queue.
// Returns: whether it was able to enqueue the value.
//
// q: the queue to add to
// author: the name of the author to add
// dist: the distance between the author's work and the inputted work
bool enqueue(PriorityQueue *q, char *author, double dist) {
    if (pq_full(q)) {
        return false;
    }
    Entry *a = (Entry *) malloc(sizeof(Entry));
    a->name = author;
    a->dist = dist;
    q->entries[q->size++] = a;
    build_heap(q->entries, 1, q->size);
    return true;
}

// Removes the lowest-distance value from the priority queue.
// Returns: whether or not it was able to dequeue the value.
//
// q: the queue to remove from
// author: a pointer to the place to store the name
// dist: a pointer to the place to store the distance
bool dequeue(PriorityQueue *q, char **author, double *dist) {
    if (pq_empty(q)) {
        return false;
    }
    Entry *a = q->entries[0];
    *author = a->name;
    *dist = a->dist;
    free(a);
    q->entries[0] = q->entries[--q->size];
    build_heap(q->entries, 1, q->size);
    return true;
}

// Debug function to print the queue.
//
// q: the queue to print
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size; i++) {
        Entry *a = q->entries[i];
        printf("%s|%f->", a->name, a->dist);
    }
    printf("\n");
    return;
}
