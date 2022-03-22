#pragma once

#include "node.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct HashTable HashTable;

typedef struct HashTableIterator HashTableIterator;

HashTable *ht_create(uint32_t size);

void ht_delete(HashTable **ht);

uint32_t ht_size(HashTable *ht);

Node *ht_lookup(HashTable *ht, char *word);

Node *ht_insert(HashTable *ht, char *word);

void ht_print(HashTable *ht);

HashTableIterator *hti_create(HashTable *ht);

void hti_delete(HashTableIterator **hti);

Node *ht_iter(HashTableIterator *hti);
