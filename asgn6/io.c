#include "io.h"
#include "code.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint8_t read_buf[BLOCK];
static int read_current = 0;
static int read_buffered = 0;

static uint8_t write_buf[BLOCK];
static int write_buffered = 0;

// Reads the specified number of bytes
// from the given input into buf.
// Returns: the number of bytes read
//
// infile: the file descriptor of the file to read from
// buf: the byte array to read into
// nbytes: the maximum number of bytes to read
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int rb = 0;
    int cur;
    while ((cur = read(infile, buf + rb, nbytes - rb)) > 0) {
        rb += cur;
    }
    bytes_read += rb;
    return rb;
}

// Writes the specified number of bytes
// from the byte array to outfile.
// Returns: the number of bytes written
//
// outfile: the file descriptor of the file to write to
// buf: the byte array to read from
// nbytes: the maximum number of bytes to write
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int wb = 0;
    int cur;
    while ((cur = write(outfile, buf + wb, nbytes - wb)) > 0) {
        wb += cur;
    }
    bytes_written += wb;
    return wb;
}

// Reads the next bit from the file
// and puts it into the bit address.
// Returns: whether the bit was read
//
// infile: the file descriptor of the file to read from
// bit: a pointer to the bit to write to
bool read_bit(int infile, uint8_t *bit) {
    // read from file if all buffer bits have been read
    if (read_current == read_buffered) {
        read_buffered = 8 * read_bytes(infile, read_buf, BLOCK);
        read_current = 0;
    }
    // no bits read or error occurred
    if (read_buffered <= 0) {
        return false;
    }
    // get bit, shift it, AND with 1 to isolate the required bit
    *bit = (read_buf[read_current / 8] >> (read_current % 8)) & 1;
    read_current++;
    return true;
}

// Writes the given code to the specified outfile.
//
// outfile: the file descriptor of the file to write to
// c: a pointer to the code to write to file
void write_code(int outfile, Code *c) {
    // iterate through each bit
    for (uint32_t i = 0; i < code_size(c); i++) {
        bool b = code_get_bit(c, i);
        // set the bit in the buffer
        if (b) {
            write_buf[write_buffered / 8] |= (1 << (write_buffered % 8));
        } else {
            write_buf[write_buffered / 8] &= ~(1 << (write_buffered % 8));
        }
        write_buffered++;
        // buffer is full
        if (write_buffered == BLOCK * 8) {
            write_bytes(outfile, write_buf, BLOCK);
            write_buffered = 0;
        }
    }
    return;
}

// Writes bits remaining in the buffer
// to the specified file.
//
// outfile: the file to write the bits to
void flush_codes(int outfile) {
    // ceiling functions make my brain hurt
    int ceil = 8 * (write_buffered / 8 + (write_buffered % 8 == 0 ? 0 : 1));
    // set extra bits to 0
    for (int i = write_buffered; i < ceil; i++) {
        write_buf[i / 8] &= ~(1 << (i % 8));
    }
    write_bytes(outfile, write_buf, ceil / 8);
    write_buffered = 0;
    return;
}
