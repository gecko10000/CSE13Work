# Author Identification

This program consists of one executable, `identify`. It will take a database of texts, read through them, and sort them by similarity to the text provided via standard input.

## Usage

To compile the program, run `$ make [all/identify]`. To run, use `$ ./identify [args]`, providing a standard input.

## Flags

The program takes many flags for execution:
* `-d`: Specifies the database to parse texts from (default: `lib.db`).
* `-n`: Specifies the noise file to read from (default: `noise.txt`).
* `-k`: The number of closest matches to display (default: 5).
* `-l`: The number of noise words to read from file (default: 100).
* `-e`: Sets the distance formula to Euclidean (default).
* `-m`: Sets the distance formula to Manhattan.
* `-c`: Sets the distance formula to Cosine.
* `-v`: Enables verbose output.
* `-H`: Specifies hash table size (default: 1 << 19).
* `-B`: Specifies Bloom filter size (default: 1 << 21).
* `-h`: Shows help and usage.

## Cleaning Up

To remove the generated `.o` files and `identify`, run `$ make clean`.
