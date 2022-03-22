CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

TARGET = encode decode test
OBJECTS = pq.o node.o huffman.o io.o stack.o code.o

.PHONY: all clean format

all: $(TARGET)

# From Awano on Discord https://discord.com/channels/926211705766305842/926211706382868587/941963699382534154
$(TARGET): % : %.o $(OBJECTS)
	$(CC) -o $@ $^

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.[c,h]

clean:
	rm -f *.o $(TARGET)
