#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct PriorityQueue PriorityQueue;

PriorityQueue *pq_create(uint32_t capacity);

void pq_delete(PriorityQueue **q);

bool pq_empty(PriorityQueue *q);

bool pq_full(PriorityQueue *q);

uint32_t pq_size(PriorityQueue *q);

bool enqueue(PriorityQueue *q, char *author, double dist);

bool dequeue(PriorityQueue *q, char **author, double *dist);

void pq_print(PriorityQueue *q);
