CC = clang
# shell info from https://www.gnu.org/software/make/manual/make.html#index-shell
LIBS = $(shell pkg-config --libs gmp)
COMP = $(shell pkg-config --cflags gmp)
CFLAGS = -Wall -Wextra -Werror -Wpedantic
.PHONY: all clean format

all: encrypt decrypt keygen

keygen: keygen.o numtheory.o randstate.o rsa.o
	$(CC) -o keygen keygen.o numtheory.o randstate.o rsa.o $(LIBS)

encrypt: encrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o encrypt encrypt.o numtheory.o randstate.o rsa.o $(LIBS)

decrypt: decrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o decrypt decrypt.o numtheory.o randstate.o rsa.o $(LIBS)

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) -c encrypt.c $(COMP)

decrypt.o: decrypt.c
	$(CC) $(CFLAGS) -c decrypt.c $(COMP)

keygen.o: keygen.c
	$(CC) $(CFLAGS) -c keygen.c $(COMP)

numtheory.o: numtheory.c
	$(CC) $(CFLAGS) -c numtheory.c $(COMP)

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c randstate.c $(COMP)

rsa.o: rsa.c
	$(CC) $(CFLAGS) -c rsa.c $(COMP)

format:
	clang-format -i -style=file *.[c,h]

clean:
	rm -f *.o keygen decrypt encrypt
