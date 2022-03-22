#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <inttypes.h>

#include "io.h"
#include "huffman.h"
#include "header.h"

#define FLAG_FORMAT "   -%c %-12s %-s\n"
#define KB          1024

// Shows program usage and
// exits the program.
//
// arg0: the name of the argument used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Compresses a file using Huffman encoding.\n\n");

    printf("USAGE\n");
    printf("   %s [-i input] [-o output] [-hv]\n\n", arg0);

    printf("OPTIONS\n");
    printf(FLAG_FORMAT, 'h', "", "Displays this message for help and usage.");
    printf(FLAG_FORMAT, 'i', "file", "The file to encode (default stdin).");
    printf(FLAG_FORMAT, 'o', "file", "The encoded file to output to (default stdout).");
    printf(FLAG_FORMAT, 'v', "",
        "Enables verbose mode, printing file statistics to the error output.");
    exit(1);
    return;
}

// Attempts to open a file with the
// specified mode, and ensures it opens
// before the program continues.
// Returns: the file descriptor of the opened file
//
// name: the name of the file to open
// mode: the mode to open the file in (O_RDONLY, O_WRONLY, O_CREAT, etc.)
// arg0: the name of the argument used to run the program
int open_file(char *name, mode_t mode, char *arg0) {
    int fd = open(name, mode);
    if (fd == -1) {
        printf("Could not open file %s.\n", name);
        usage(arg0);
    }
    return fd;
}

// Attempts to seek to the
// start of the given file.
// Returns: the resulting offset (return value of lseek)
//
// fd: the file descriptor to attempt to seek
off_t seek_to_start(int fd) {
    return lseek(fd, 0, SEEK_SET);
}

// Gets and returns the file
// permissions of the given file.
// Returns: the permissions of the file
//
// fd: the file descriptor to get permission info for
mode_t get_permissions(int fd) {
    struct stat stats;
    fstat(fd, &stats);
    mode_t mode = stats.st_mode;
    return mode;
}

// Calculates the byte size of a
// Huffman tree given a histogram.
// Returns: the size of the tree in bytes
//
// histogram: the array recording the number of appearances of each char
uint16_t tree_size(uint64_t histogram[static ALPHABET]) {
    uint16_t nonzero = 0;
    for (int i = 0; i < ALPHABET; i++) {
        if (histogram[i] > 0) {
            nonzero++;
        }
    }
    return nonzero * 3 - 1;
}

// Gets and returns the
// size of the given file.
// Returns: the size of the file in bytes
//
// fd: the file descriptor to get size for
uint64_t get_size(int fd) {
    struct stat stats;
    fstat(fd, &stats);
    return stats.st_size;
}

int main(int argc, char *argv[]) {
    // defaults
    char *infn = NULL;
    char *outfn = NULL;
    bool verbose = false;

    // parse options
    int option;
    while ((option = getopt(argc, argv, "i:o:vh")) != -1) {
        switch (option) {
        case 'i':
            if (infn != NULL) {
                free(infn);
            }
            infn = strdup(optarg);
            break;
        case 'o':
            if (outfn != NULL) {
                free(outfn);
            }
            outfn = strdup(optarg);
            break;
        case 'v': verbose = true; break;
        case 'h':
        default: usage(argv[0]);
        }
    }

    int infile = fileno(stdin);
    int outfile = fileno(stdout);
    if (infn != NULL) {
        infile = open_file(infn, O_RDONLY, argv[0]);
        free(infn);
    }
    if (outfn != NULL) {
        outfile = open_file(outfn, O_CREAT | O_WRONLY, argv[0]);
        free(outfn);
    }
    uint8_t *buf = (uint8_t *) malloc(BLOCK * sizeof(uint8_t));
    // copy contents to a file if the input is not seekable
    bool using_temp = false;
    if (seek_to_start(infile) == -1) { // could not seek
        infn = (char *) malloc(sizeof(char) * KB);
        snprintf(infn, KB, "/tmp/encode.%u",
            getpid()); // inspired by Elmer on Discord https://discord.com/channels/926211705766305842/926211706382868584/946648901983809576
        using_temp = true;
        int tmp = creat(infn, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (tmp == -1) {
            printf("Could not create temp file %s.\n", infn);
            usage(argv[0]);
        }
        int read_in;
        while ((read_in = read_bytes(infile, buf, BLOCK)) > 0) {
            write_bytes(tmp, buf, read_in);
        }
        fchmod(tmp, get_permissions(infile));
        // close and re-open in read mode
        close(tmp);
        infile = open_file(infn, O_RDONLY, argv[0]);
    }

    // populate histogram
    uint64_t *histogram = (uint64_t *) calloc(ALPHABET, sizeof(uint64_t));
    int read_in;
    while ((read_in = read_bytes(infile, buf, BLOCK)) > 0) {
        for (int i = 0; i < read_in; i++) {
            histogram[buf[i]]++;
        }
    }
    histogram[0]++;
    histogram[255]++;

    // build the tree
    Node *tree = build_tree(histogram);

    // create array of codes
    Code *table = (Code *) calloc(ALPHABET, sizeof(Code));
    build_codes(tree, table);

    // create header
    Header h;
    h.magic = MAGIC;
    h.permissions = get_permissions(infile);
    fchmod(outfile, h.permissions); // equalize output file permissions
    h.tree_size = tree_size(histogram);
    h.file_size = get_size(infile);

    bytes_written = 0;
    // writes header to file
    write_bytes(outfile, (uint8_t *) &h, sizeof(h));

    // puts tree in file
    dump_tree(outfile, tree);

    // writes each character's code to the file
    seek_to_start(infile);
    while ((read_in = read_bytes(infile, buf, BLOCK)) > 0) {
        for (int i = 0; i < read_in; i++) {
            write_code(outfile, &table[buf[i]]);
        }
    }
    flush_codes(outfile);

    // print info to error if enabled
    if (verbose) {
        fprintf(stderr, "Uncompressed file size: %" PRIu64 " bytes\n", h.file_size);
        fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", bytes_written);
        fprintf(
            stderr, "Space saving: %4.2f%%\n", 100 * (1 - bytes_written / (double) h.file_size));
    }

    close(infile);
    // remove temp file if it was created
    if (using_temp) {
        free(infn);
    }
    close(outfile);
    delete_tree(&tree);
    free(buf);
    free(table);
    free(histogram);
    return 0;
}
