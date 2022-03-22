CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: collatz

collatz: collatz.o
	$(CC) -o collatz collatz.o

collatz.o: collatz.c
	$(CC) $(CFLAGS) -c collatz.c

clean:
	rm -f collatz *.o

format:
	clang-format -i -style=file *.{c,h}
