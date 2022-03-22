# The Game of Life

This program simulates the Game of Life and runs a finite number of generations before saving the final configuration to either standard output or a file.

## Usage

To compile the program, run `$ make [all/life]`. This will create the `life` executable, which can be executed with `$./life [args]`.

## Input

To create the initial universe, there needs to be a certain number of parameters in the input (whether in standard input or via a file). These parameters are given in the following order:
* Number of rows in the universe
* Number of columns in the universe
* The locations of all cells that should start alive (in "row column" format)

## Ouput

The output of the program, the resulting universe, goes either directly to a file, or to the standard output. It is a rectangle of characters, with the width and height defined by the number of columns and rows in the universe. Dead cells in the universe are represented with `.`, while live cells are shown with lowercase `o`.

## Arguments

A more detailed explanation of the options can be brought up with the `-H` flag.The `-t` flag can be used to define the universe (environment in which the Game of Life runs) as toroidal, meaning the edges wrap around to each other. Using `-s` will run the simulation without using `ncurses` to print a visualization of the generations. Using `-n` with an amount will define the number of generations to simulate (100 by default). The `-i` and `-o` flags will define the input and output files, which are the standard input and standard output by default.

## Cleaning Up

To remove generated `.o` files and the `life` executable, use `$ make clean`.
