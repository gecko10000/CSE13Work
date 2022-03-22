#include "parser.h"
#include <regex.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK 4096

static inline int min(int x, int y) {
    return x < y ? x : y;
}

// We can store a block of words, each a block number of characters.
// This is generally okay for the English language.
static char words[BLOCK][BLOCK];

char *next_word(FILE *infile, regex_t *word_regex) {
    static uint32_t index = 0; // Track the word to return.
    static uint32_t count = 0; // How many words have we stored?
    static char buffer[BLOCK]; // Internal buffer to read into.
    static char *cursor; // Tracks position in buffer.
    static regmatch_t match; // Struct to track regex matches.

    if (!index) {
        int matches = 0; // Assume no matches yet.
        cursor = buffer; // Reset buffer cursor.

        // Loop while we have no matches.
        while (!matches) {
            // No more possible matches in the file.
            if (!fgets(buffer, BLOCK, infile)) {
                return NULL;
            }

            // Fill the word buffer with as many matches as possible.
            for (int i = 0; i < BLOCK; i += 1) {
                // From the cursor, look for 1 match without extended regex flags.
                if (regexec(word_regex, cursor, 1, &match, 0)) {
                    break;
                }

                // No match in the line.
                if (match.rm_so < 0) {
                    break;
                }

                // Range for the match.
                int start = match.rm_so;
                int end = match.rm_eo;
                int length = end - start;

                // Copy it in, tack on null-terminator.
                memcpy(words[i], cursor + start, min(length, BLOCK));
                words[i][length] = '\0';
                cursor += end;
                matches += 1;
            }

            // Store number of matches.
            count = matches;
        }
    }

    char *word = words[index];
    index = (index + 1) % count;
    return word;
}
