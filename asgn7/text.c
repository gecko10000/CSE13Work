#include <inttypes.h>
#include <regex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#include "metric.h"
#include "ht.h"
#include "bf.h"
#include "parser.h"
#include "text.h"

#define REGEX "[a-zA-Z]+([a-zA-Z'-][a-zA-Z]+)*"

uint32_t noiselimit = 100, hash_table_size = (1 << 19), bloom_filter_size = (1 << 21);

uint64_t bf_false_positives = 0, bf_lookups = 0;

// adapted from assignment
struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

// Creates a text from the given file, filtering out the given noise.
// Returns: a pointer to the created text.
//
// infile: the file to read from
// noise: the noise to ignore
Text *text_create(FILE *infile, Text *noise) {
    Text *text = (Text *) malloc(sizeof(Text));
    text->ht = ht_create(hash_table_size);
    text->bf = bf_create(bloom_filter_size);
    text->word_count = 0;
    if (text->ht == NULL || text->bf == NULL) {
        fprintf(stderr, "Could not allocate memory for text.\n");
        free(text);
        return NULL;
    }
    // adapted from assignment
    regex_t regex;
    if (regcomp(&regex, REGEX, REG_EXTENDED)) {
        fprintf(stderr, "Regex could not compile.\n");
        free(text);
        return NULL;
    }

    char *word;
    while ((word = next_word(infile, &regex)) != NULL) {
        for (char *c = word; *c != '\0'; c++) {
            *c = tolower(*c);
        }
        // checks for NULL too, don't need to check that noise == NULL
        if (text_contains(noise, word)) {
            continue;
        }
        if (!ht_insert(text->ht, word)) {
            fprintf(stderr, "Hash table is full\n");
            break;
        }
        bf_insert(text->bf, word);
        text->word_count++;
        // if we're creating noise and we reached the limit
        if (noise == NULL && text->word_count >= noiselimit) {
            break;
        }
    }
    regfree(&regex);
    return text;
}

// Deletes the specified text.
//
// text: a pointer to the address of the text to delete
void text_delete(Text **text) {
    ht_delete(&(*text)->ht);
    bf_delete(&(*text)->bf);
    free(*text);
    *text = NULL;
    return;
}

// Calculates the distance between two words' frequencies.
// Returns: the distance, depending on the metric used.
//
// f1: the first value
// f2: the second value
// metric: the distance algorithm to use for calculations
static double freq_dist(double f1, double f2, Metric metric) {
    switch (metric) {
    case MANHATTAN: return fabs(f1 - f2);
    case EUCLIDEAN: return pow(f1 - f2, 2);
    case COSINE: return f1 * f2;
    default: fprintf(stderr, "Unknown Metric used.\n"); return 0;
    }
}

// Returns the "distance" between two computed vectors, composed of the words in the texts.
//
// text1: the first text to read the words from
// text2: the second text to read the words from
// metric: the algorithm to use for the calculations
double text_dist(Text *text1, Text *text2, Metric metric) {
    double total = 0;

    // loop over all words for text1
    HashTableIterator *hti1 = hti_create(text1->ht);
    Node *n;
    while ((n = ht_iter(hti1)) != NULL) {
        double f1 = text_frequency(text1, n->word);
        double f2 = text_frequency(text2, n->word);
        total += freq_dist(f1, f2, metric);
    }
    hti_delete(&hti1);
    // loop over text2, but ignore words already done in 1
    HashTableIterator *hti2 = hti_create(text2->ht);
    while ((n = ht_iter(hti2)) != NULL) {
        // ignore duplicates
        if (text_contains(text1, n->word)) {
            continue;
        }
        double f2 = text_frequency(text2, n->word);
        total += freq_dist(0, f2, metric); // we know the word is not in text1
    }
    hti_delete(&hti2);

    // now apply appropriate steps
    switch (metric) {
    case MANHATTAN: return total; // already done
    case EUCLIDEAN: return sqrt(total);
    case COSINE: return 1 - total;
    default: fprintf(stderr, "Unknown Metric used.\n"); return -1;
    }
}

// Calculates the normalized frequency of a word in a text.
// Returns: the normalized frequency.
//
// text: the text to find the occurrences of the word in
// word: the word to look for
double text_frequency(Text *text, char *word) {
    if (!text_contains(text, word)) {
        return 0;
    }
    Node *n = ht_lookup(text->ht, word);
    return n == NULL ? 0 : n->count / (double) text->word_count;
}

// Returns whether or not the text contains the given word.
//
// text: the text to check
// word: the word to look for
bool text_contains(Text *text, char *word) {
    if (text == NULL) {
        return false;
    }
    bf_lookups++;
    if (!bf_probe(text->bf, word)) {
        return false;
    }
    bool contains = ht_lookup(text->ht, word) != NULL;
    // BF told us it's there, but it's not
    if (!contains) {
        bf_false_positives++;
    }
    return contains;
}

// Debug function to print the text.
//
// text: the text to print
void text_print(Text *text) {
    bf_print(text->bf);
    ht_print(text->ht);
    return;
}
