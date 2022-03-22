CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -Ofast
OFLAGS = -lm

TARGET = identify
OBJECTS = bf.o bv.o ht.o node.o parser.o pq.o speck.o text.o

.PHONY: all clean format

all: $(TARGET)

debug: OFLAGS += -g -pg
debug: CC := gcc
debug: clean all

$(TARGET): % : %.o $(OBJECTS)
	$(CC) -o $@ $^ $(OFLAGS)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.[c,h]

clean:
	rm -rf *.o gmon.out ./$(TARGET)
