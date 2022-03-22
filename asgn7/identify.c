#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "metric.h"
#include "pq.h"
#include "text.h"

#define FLAG_FORMAT "   -%c %-12s %-s\n"
#define MAX_STRING  100

// for statistics
extern uint64_t ht_lookups;
extern uint64_t lookup_probes;
extern uint64_t ht_insertions;
extern uint64_t insertion_probes;
extern uint64_t bf_false_positives;
extern uint64_t bf_lookups;
// reset for every hash table creation
extern uint64_t used_slots;

extern uint32_t hash_table_size, bloom_filter_size;

// Shows program usage and exits the program.
//
// arg0: the command used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Identifies the most likely author of a text.\n\n");

    printf("USAGE\n");
    printf("   %s [-d database] [-n noise] [-k matches] [-l noise_amount] [-e|m|c] [-v] [-H size] "
           "[-B size] [-h]\n\n",
        arg0);

    printf("OPTIONS\n");
    printf(FLAG_FORMAT, 'd', "database",
        "Sets the name of the database containing the texts to analyze. (default: lib.db)");
    printf(FLAG_FORMAT, 'n', "noise_file",
        "Sets the noise file (used to specify words to filter). (default: noise.txt)");
    printf(FLAG_FORMAT, 'k', "matches", "Sets the number of top matches to display. (default: 5)");
    printf(FLAG_FORMAT, 'l', "noise_words",
        "Sets the number of noise words to read from the file. (default: 100)");
    printf(FLAG_FORMAT, 'e', "",
        "Sets the distance formula to use Euclidean distance. This is the default.");
    printf(FLAG_FORMAT, 'm', "", "Sets the distance formula to use Manhattan distance.");
    printf(FLAG_FORMAT, 'c', "", "Sets the distance formula to use Cosine distance.");
    printf(FLAG_FORMAT, 'v', "", "Enables verbose output.");
    printf(FLAG_FORMAT, 'H', "size", "Specifies the hash table size (default 1 << 19).");
    printf(FLAG_FORMAT, 'B', "size", "Specifies the Bloom filter size (default 1 << 21).");
    printf(FLAG_FORMAT, 'h', "", "Shows this message for program help and usage.");
    exit(1);
    return;
}

// Opens the given file for reading, or exits if it couldn't.
// Returns: a file pointer to the opened file.
//
// name: the name of the file to open
// arg0: the command used to run the program, for use when calling usage
FILE *open_read(char *name, char *arg0) {
    FILE *f = fopen(name, "r");
    if (f == NULL) {
        printf("File %s not found.\n", name);
        usage(arg0);
    }
    return f;
}

int main(int argc, char *argv[]) {
    // defaults
    char *db_name = "lib.db";
    char *noise_file_name = "noise.txt";
    uint32_t matches = 5;
    Metric metric = EUCLIDEAN;
    bool verbose = false;

    // parse options
    int option;
    while ((option = getopt(argc, argv, "d:n:k:l:emcvH:B:h")) != -1) {
        switch (option) {
        case 'd': db_name = optarg; break;
        case 'n': noise_file_name = optarg; break;
        case 'k': matches = strtoul(optarg, NULL, 10); break;
        case 'l': noiselimit = strtoul(optarg, NULL, 10); break;
        case 'e': metric = EUCLIDEAN; break;
        case 'm': metric = MANHATTAN; break;
        case 'c': metric = COSINE; break;
        case 'v': verbose = true; break;
        case 'H': hash_table_size = strtoul(optarg, NULL, 10); break;
        case 'B': bloom_filter_size = strtoul(optarg, NULL, 10); break;
        case 'h':
        default: usage(argv[0]); break;
        }
    }

    FILE *database = open_read(db_name, argv[0]);
    FILE *noise_file = open_read(noise_file_name, argv[0]);

    // create noise
    Text *noise_text = text_create(noise_file, NULL);
    fclose(noise_file);

    Text *anon_text = text_create(stdin, noise_text);

    uint32_t texts;
    if (!fscanf(database, "%" SCNu32 "\n", &texts)) {
        fprintf(stderr, "Could not scan the number of texts from %s.\n", db_name);
        return 1;
    }

    PriorityQueue *pq = pq_create(texts);

    char *text_author = (char *) calloc(MAX_STRING, sizeof(char));
    char *text_path = (char *) calloc(MAX_STRING, sizeof(char));
    double average_ht_load = 0; // used to calculate the average fill of the HT
    double max_ht_load = 0;
    for (uint32_t i = 0; i < texts; i++) {
        if (!fgets(text_author, MAX_STRING, database) || !fgets(text_path, MAX_STRING, database)) {
            fprintf(stderr, "Could not scan database entry #%" PRIu32 ".\n", i + 1);
            free(text_author);
            free(text_path);
            return 1;
        }
        text_author[strlen(text_author) - 1] = '\0'; // remove newlines
        text_path[strlen(text_path) - 1] = '\0';
        // don't use open_read because we don't
        // want to exit if the file couldn't be opened
        FILE *text_file = fopen(text_path, "r");
        if (text_file == NULL) {
            fprintf(stderr, "File %s could not be opened.\n", text_path);
            continue;
        }
        // creates and fills the hash table, updating used_slots
        Text *text = text_create(text_file, noise_text);
        double load = used_slots / (double) hash_table_size;
        average_ht_load += load;
        if (load > max_ht_load) {
            max_ht_load = load;
        }
        double dist = text_dist(text, anon_text, metric);
        text_delete(&text);
        enqueue(pq, strdup(text_author), dist);
        fclose(text_file);
    }
    average_ht_load /= texts;
    fclose(database);
    text_delete(&noise_text);
    text_delete(&anon_text);
    free(text_author);
    free(text_path);

    char *author;
    double dist;
    printf("Top %" PRIu32 ", metric: %s, noise limit: %" PRIu32 "\n", matches, metric_names[metric],
        noiselimit);
    for (uint32_t i = 1; i <= matches && dequeue(pq, &author, &dist); i++) {
        printf("%" PRIu32 ") %s [%17.15f]\n", i, author, dist);
        free(author);
    }
    pq_delete(&pq);
    if (verbose) {
        printf("\n");
        printf("Average Probes per Insertion: %f\n", insertion_probes / (double) ht_insertions);
        printf("Average Probes per Lookup: %f\n", lookup_probes / (double) ht_lookups);
        printf("Average Hash Table Load: %f\n", average_ht_load);
        printf("Max Hash Table Load: %f\n", max_ht_load);
        printf("Bloom Filter False Positive Rate: %f\n", bf_false_positives / (double) bf_lookups);
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        int64_t sec = usage.ru_utime.tv_sec;
        int64_t ms = (usage.ru_utime.tv_usec + 500) / 1000;
        if (ms >= 1000) {
            sec++;
            ms -= 1000;
        }
        printf("Seconds taken: %" PRId64 ".%03" PRId64 "\n", sec, ms); // round to nearest ms
    }
    return 0;
}
