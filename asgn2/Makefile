CC= clang
CFLAGS= -Wall -Wextra -Werror -Wpedantic

.PHONY: all clean format test
all: integrate

integrate: integrate.o functions.o mathlib.o
	$(CC) -o integrate integrate.o functions.o mathlib.o

test: test.o functions.o mathlib.o
	$(CC) -o test test.o functions.o mathlib.o -lm

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

integrate.o: integrate.c
	$(CC) $(CFLAGS) -c integrate.c

functions.o: functions.c
	$(CC) $(CFLAGS) -c functions.c

mathlib.o: mathlib.c
	$(CC) $(CFLAGS) -c mathlib.c

clean:
	rm -f integrate test *.o

format:
	clang-format -i -style=file *.[c,h]
