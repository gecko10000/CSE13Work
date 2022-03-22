#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <inttypes.h>

#include "huffman.h"
#include "header.h"
#include "io.h"
#include "defines.h"

#define FLAG_FORMAT "   -%c %-12s %-s\n"

// Shows program usage and
// exits the program.
//
// arg0: the name of the argument used to run the program
void usage(char *arg0) {
    printf("SYNOPSIS\n");
    printf("   Decompresses an encoded file using Huffman encoding.\n\n");

    printf("USAGE\n");
    printf("   %s [-i input] [-o output] [-hv]\n\n", arg0);

    printf("OPTIONS\n");
    printf(FLAG_FORMAT, 'h', "", "Displays this message for help and usage.");
    printf(FLAG_FORMAT, 'i', "file", "The encoded file to decode (default stdin).");
    printf(
        FLAG_FORMAT, 'o', "file", "The file to output the decrypted content to (default stdout).");
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

    // Read header
    Header h;
    read_bytes(infile, (uint8_t *) &h, sizeof(Header));
    // Validate magic number
    if (h.magic != MAGIC) {
        printf("Magic number does not match; this probably means the input was not encoded "
               "properly.\n");
        usage(argv[0]);
    }
    fchmod(outfile, h.permissions);

    // Read encoded tree and rebuild it
    uint8_t encoded_tree[h.tree_size];
    read_bytes(infile, encoded_tree, h.tree_size);
    Node *root = rebuild_tree(h.tree_size, encoded_tree);

    Node *cur = root;
    uint8_t buf[BLOCK];
    int buf_bytes = 0;
    // for reading bit from infile
    uint8_t bit;
    // add written and buffered when comparing
    while (bytes_written + buf_bytes < h.file_size) {
        // leaf node
        if (cur == NULL) {
            cur = root;
            continue;
        }
        if (cur->left == NULL && cur->right == NULL) {
            buf[buf_bytes++] = cur->symbol;
            // write if buffer is full
            if (buf_bytes >= BLOCK) {
                buf_bytes = 0;
                write_bytes(outfile, buf, BLOCK);
            }
            // reset cur to top of tree
            cur = root;
            continue;
        }
        read_bit(infile, &bit);
        // go left for 0, right for 1
        if (bit) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }
    write_bytes(outfile, buf, buf_bytes);

    // Print stats if -v was specified
    if (verbose) {
        fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", bytes_read);
        fprintf(stderr, "Decompressed file size: %" PRIu64 " bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %4.2f%%\n", 100 * (1 - bytes_read / (double) bytes_written));
    }
    close(infile);
    close(outfile);
    delete_tree(&root);
    return 0;
}
