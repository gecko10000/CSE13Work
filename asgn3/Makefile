CC= clang
CFLAGS= -Wall -Wextra -Werror -Wpedantic

.PHONY: all clean format
all: sorting

sorting: sorting.o batcher.o heap.o insert.o quick.o stats.o
	$(CC) -o sorting sorting.o batcher.o heap.o insert.o quick.o stats.o

sorting.o: sorting.c
	$(CC) $(CFLAGS) -c sorting.c

batcher.o: batcher.c
	$(CC) $(CFLAGS) -c batcher.c

heap.o: heap.c
	$(CC) $(CFLAGS) -c heap.c

insert.o: insert.c
	$(CC) $(CFLAGS) -c insert.c

quick.o: quick.c
	$(CC) $(CFLAGS) -c quick.c

stats.o: stats.c
	$(CC) $(CFLAGS) -c stats.c

clean:
	rm -f sorting *.o

format:
	clang-format -i -style=file *.[c,h]
