#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ht.h"
#include "salts.h"
#include "node.h"
#include "speck.h"

uint64_t ht_lookups = 0, lookup_probes = 0, ht_insertions = 0, insertion_probes = 0, used_slots = 0;

// copied from assignment
struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

// Creates a hash table of the given size.
// Returns: a pointer to the hash table.
//
// size: the size of the array used for the hash table
HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    used_slots = 0;
    ht->salt[0] = SALT_HASHTABLE_LO;
    ht->salt[1] = SALT_HASHTABLE_HI;
    ht->size = size;
    ht->slots = (Node **) calloc(size, sizeof(Node *));
    if (ht->slots == NULL) {
        free(ht);
        return NULL;
    }
    return ht;
}

// Deletes the hash table.
//
// ht: a pointer to the address of the hash table
void ht_delete(HashTable **ht) {
    for (uint32_t i = 0; i < (*ht)->size; i++) {
        if ((*ht)->slots[i] == NULL) {
            continue;
        }
        node_delete(&((*ht)->slots[i]));
    }
    free((*ht)->slots);
    free(*ht);
    *ht = NULL;
    return;
}

// Returns the size of the hash table.
//
// ht: the hash table to get the size of
uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

// Attempts to find the given word in the hash table.
// Returns: the found node containing the word, or NULL if it was not found.
//
// ht: the hash table to look through
// word: the word to hash and search for
Node *ht_lookup(HashTable *ht, char *word) {
    uint32_t index = hash(ht->salt, word) % ht->size;
    uint32_t original = index;
    ht_lookups++;
    while (
        ++lookup_probes && ht->slots[index] != NULL && strcmp(ht->slots[index]->word, word) != 0) {
        index = (index + 1) % ht->size; // continue until empty index is found or word is found
        if (index == original) { // we have looped back to the original
            return NULL; // not in table
        }
    }
    return ht->slots[index]; // return found Node *
}

// Attempts to insert the given word into the hash table.
// Returns: the created node, or NULL if there was no room left.
//
// ht: the hash table to insert into
// word: the word to hash and insert into the hash table
Node *ht_insert(HashTable *ht, char *word) {
    uint32_t index = hash(ht->salt, word) % ht->size;
    uint32_t original = index;
    ht_insertions++;
    while (++insertion_probes && ht->slots[index] != NULL
           && strcmp(ht->slots[index]->word, word) != 0) {
        index = (index + 1) % ht->size;
        if (index == original) { // back to start
            return NULL; // could not be inserted
        }
    }
    if (ht->slots[index] == NULL) { // need to create node if null
        ht->slots[index] = node_create(word);
        used_slots++;
    }
    ht->slots[index]->count++;
    return ht->slots[index];
}

// Debug function to print the hash table.
//
// ht: the hash table to print
void ht_print(HashTable *ht) {
    printf("Salts: %" PRIu64 " %" PRIu64 "\n", ht->salt[0], ht->salt[1]);
    printf("Size: %" PRIu32 "\n", ht->size);
    printf("Slots: ");
    for (uint32_t i = 0; i < ht->size; i++) {
        node_print(ht->slots[i]);
    }
    return;
}

// also copied from assignment
struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
};

// Creates a hash table iterator from the given table.
// Returns: a pointer to the iterator.
//
// ht: the hash table to iterate over
HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(HashTableIterator));
    hti->table = ht;
    hti->slot = 0;
    return hti;
}

// Deletes the give hash table iterator.
//
// hti: a pointer to the address of the hash table iterator to delete
void hti_delete(HashTableIterator **hti) {
    free(*hti);
    hti = NULL;
    return;
}

// Goes to the next existing entry in the hash table.
// Returns: the next entry, or NULL if the end of the table was reached.
//
// hti: the iterator to advance
Node *ht_iter(HashTableIterator *hti) {
    if (hti->slot >= hti->table->size) {
        return NULL;
    }
    Node *next = hti->table->slots[hti->slot++];
    while (next == NULL && hti->slot < hti->table->size) {
        next = hti->table->slots[hti->slot++];
    }
    return next;
}
