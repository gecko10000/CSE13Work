# Numerical Integration

This program approximates integrals by calculating over an interval with the Composite Simpson's 1/3 Rule.

## Usage

To compile, run `$ make integrate`. This will generate the `integrate` executable, which can be run with `$ ./integrate <args>`.

The arguments are further explained in the program's help message (`./integrate -H`), but the `a`-`j` flags will specify a formula to use, while `-n (value)` will specify the number of partitions. The `p` and `q` flags (followed by a double) will specify the low and high values of the interval, respectively.

To remove generated files, run `$ make clean`.

To create the plots used in `WRITEUP.pdf`, use `$ ./plot.sh`.
