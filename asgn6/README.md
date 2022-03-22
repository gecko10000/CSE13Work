# Huffman Coding

This program consists of two executables, `encode` and `decode`. These will compress and decompress the given file accordingly, whether by standard input or by specifying a file.

## Usage

To compile both programs, run `$ make [all]`. Each can be compiled separately with `$ make encode|decode`. To run them, use `$ ./encode|decode [args]`.

## Flags

Both programs take the same set of flags for program execution:
* `-i`: Specifies the file to use as input (to encode in `encode` and decode in `decode`). Defaults to `stdin`.
* `-o`: Specifies the file to use as output (the encoded file in `encode` and the decoded file in `decode`). Defaults to `stdout`.
* `-v`: Enables verbose mode, printing statistics about the files before and after encoding/decoding to `stderr`.

## Cleaning Up

To remove generated `.o` files, `encode`, and `decode`, run `$ make clean`.
