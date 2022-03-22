# Sorting: Putting Your Affairs in Order

This program generates pseudorandom numbers and sorts them using one or more of the following sorting algorithms:
* Insertion Sort
* Heap Sort
* Quick Sort
* Batcher's Odd-Even Merge Sort

## Usage

To compile the program, run `$ make sorting`. This will generate the `sorting` executable, which can be run with `$ ./sorting [args]`.

## Arguments

The arguments are explained in more detail with the `-H` flag. Using `-i`, `-h`, `-q`, or `-b` will specify Insertion, Heap, Quick, or Batcher sort respectively. Multiple can be enabled at once, and `-a` will enable them all. The `-r` flag will specify the seed used for the pseudorandom number-generating algorithm, `-n` will specify the size of the array to generate and sort, and `-p` will specify the number of sorted numbers to print.

## Cleaning Up

To remove the `.o` files and `sorting`, run `$ make clean`.
