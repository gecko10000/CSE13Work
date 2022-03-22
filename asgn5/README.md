# Public Key Cryptography

This program consists of three main executables, allowing the user to create an RSA key combination, encrypt files with the public key, and decrypt them with the private key.

## Usage

To compile all the programs, run `$ make [all]`. Each one can also be compiled separately with `$ make <keygen|encrypt|decrypt>`. The executables can be run with `$ ./keygen|encrypt|decrypt [args]`.

## Input

All three programs take optional flags specifying different parameters for program execution. The `keygen` program takes the following:
* `-b`: allows the user to define the minimum number of bits for the public modulus (defaults to 256)
* `-i`: the number of iterations for testing primes (defaults to 50)
* `-n`/`-d`: specifies the public and private key files, respectively (defaults to `rsa.pub` and `rsa.priv`)
* `-s`: the seed for the random number generation (defaults to current system time)
* `-v`: enables verbose output
* `-h`: show help for the program

The `encrypt` and `decrypt` programs take a number of flags as well:
* `-i` and `-o`: specifies the input and output file names (default to standard input and output)
* `-n`: specifies the filename for the public/private key (default `rsa.pub`/`rsa.priv`)
* `-v`: enables verbose output
* `-h`: show help for the program

## Cleaning Up

To remove generated `.o` files, `keygen`, `encrypt`, and `decrypt`, run `$ make clean`.
