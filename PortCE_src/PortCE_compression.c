/**
 * @file
 *
 * @author ZX7 decoder by Einar Saukas & Urusergi
 * https://github.com/antoniovillena/zx7b/blob/master/dzx7b.c
 */

#include <stdint.h>
#include <stdio.h>

unsigned char *input_data;
unsigned char *output_data;
size_t input_index;
size_t output_index;
size_t input_size;
size_t output_size;
size_t partial_counter;
size_t total_counter;
int bit_mask;
int bit_value;

static int read_byte() {
    return input_data[input_index++];
}

static int read_bit() {
    bit_mask >>= 1;
    if (bit_mask == 0) {
        bit_mask = 128;
        bit_value = read_byte();
    }
    return bit_value & bit_mask ? 1 : 0;
}

static int read_elias_gamma() {
    __attribute__((unused)) int i;
    int value;

    value = 1;
    while (!read_bit()) {
        value = value << 1 | read_bit();
    }
    if( (value&255)==255 )
      value= -1;
    return value;
}

static int read_offset() {
    int value;
    int i;

    value = read_byte();
    if (value < 128) {
        return value;
    } else {
        i = read_bit();
        i = i << 1 | read_bit();
        i = i << 1 | read_bit();
        i = i << 1 | read_bit();
        return ((value & 127) | i << 7) + 128;
    }
}

static void write_byte(int value) {
    output_data[output_index++] = value;
}

static void write_bytes(int offset, int length) {
    if ((size_t)offset > output_size + output_index) {
        #ifndef _EZ80
        // fprintf(stderr, "Error: Invalid data in input file %s\n", input_name);
        // exit(1);
        printf("zx7_Decompress Error: Invalid data in input file\n");
        #endif
    }
    while (length-- > 0) {
        write_byte(output_data[output_index-offset]);
    }
}

static void decompress(void) {
    int length;
    __attribute__((unused)) int i;

    input_index = 0;
    partial_counter = 0;
    output_index = 0;
    bit_mask = 0;

    write_byte(read_byte());
    while (1) {
        if (!read_bit()) {
            write_byte(read_byte());
        } else {
            length = read_elias_gamma()+1;
            if (length == 0) {
                return;
            }
            write_bytes(read_offset()+1, length);
        }
    }
}

void zx7_Decompress(void *dst, const void *src) {
    output_data = (unsigned char*)dst;
    input_data = (unsigned char*)src;
    decompress();
}
