#include <compression.h>
#include <stdint.h>
#include <stdio.h>

typedef union reg {
    uint16_t full;
    struct {
        uint8_t low;
        uint8_t high;
    };
} reg;

static uint8_t* dst;
static const uint8_t* src;
static uint8_t a;
static bool carry;
static bool zero;
static reg reg_bc;
static reg reg_de;

#define bc reg_bc.full
#define c  reg_bc.low
#define b  reg_bc.high

#define de reg_de.full
#define e  reg_de.low
#define d  reg_de.high

#define call(cond, func) if ((cond)) { func(); }

#define jr(cond, label) if ((cond)) { goto label; }

static void ccf() {
    carry = !carry;
}

static void add_a_a() {
    carry = (a & 0x80);
    a <<= 1;
    zero = (a == 0);
}

static void sla(uint8_t& r) {
    carry = (r & 0x80);
    r <<= 1;
    zero = (r == 0);
}

static void rl(uint8_t& r) {
    bool next = (r & 0x80);
    r <<= 1;
    if (carry) { r |= 0x01; }
    carry = next;
    zero = (r == 0);
}

static void rr(uint8_t& r) {
    bool next = (r & 0x01);
    r >>= 1;
    if (carry) { r |= 0x80; }
    carry = next;
    zero = (r == 0);
}

static void inc(uint8_t& r) {
    ++r;
    zero = (r == 0);
}

static void dec(uint8_t& r) {
    --r;
    zero = (r == 0);
}

static void rla() {
    bool next = (a & 0x80);
    a <<= 1;
    if (carry) { a |= 0x01; }
    carry = next;
}

//------------------------------------------------------------------------------
// ZX7 decoder by Einar Saukas & Urusergi
// "Turbo" version
//------------------------------------------------------------------------------

static void zx7_load_bits(void) {
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
}

void zx7_Decompress(void *arg_dst, const void *arg_src) {
    dst = (uint8_t*)arg_dst;
    src = (const uint8_t*)arg_src;
    if (arg_dst == nullptr || arg_src == nullptr) {
        fprintf(stderr, "zx7_Decompress: nullptr arguments: dst(%p) src(%p)\n", dst, src);
        return;
    }

    a = 0x80;

dzx7t_copy_byte_loop:
    // copy literal byte
    /* LDI */ {
        *dst = *src;
        ++dst;
        ++src;
        --bc;
    }
dzx7t_main_loop:
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);
    // next bit indicates either literal or sequence
    jr(!carry, dzx7t_copy_byte_loop);
    // determine number of bits used for length (Elias gamma coding)

    // push de
    de = 0;
    bc = 1;
dzx7t_len_size_loop:
    inc(d);
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);
    jr(!carry, dzx7t_len_size_loop);
    goto dzx7t_len_value_start;

    // determine length

dzx7t_len_value_loop:
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);

    rl(c);
    rl(b);
    // check end marker
    jr(carry, dzx7t_exit);
dzx7t_len_value_start:
    dec(d);
    jr(!zero, dzx7t_len_value_loop);
    // adjust length
    ++bc;
    // determine offset
    // load offset flag (1 bit) + offset value (7 bits)
    e = *src;
    ++src;
    sla(e);
    inc(e);
    // if offset flag is set, load 4 extra bits
    jr(!carry, dzx7t_offset_end);
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);
    // insert first bit into D
    rl(d);
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);
    // insert second bit into D
    rl(d);
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);
    // insert third bit into D
    rl(d);
    // check next bit
    add_a_a();
    // no more bits left?
    call(zero, zx7_load_bits);
    ccf();
    jr(carry, dzx7t_offset_end);
    // equivalent to adding 128 to DE
    inc(d);
dzx7t_offset_end:
    // insert inverted fourth bit into E
    rr(e);
    // copy previous sequence
    // store source, restore destination

    // ex (sp), hl
    // store destination
    // push hl
    // HL = destination - offset - 1

    // sbc hl, de
    // DE = destination
    // pop de
    carry = false;
    /* LDIR */ do {
        *dst = *(dst - de - 1);
        ++dst;
    } while (--bc != 0);
dzx7t_exit:
    // restore source address (compressed data)
    // pop hl
    jr(!carry, dzx7t_main_loop);
    return;
    // we should probably not read out of bounds like the eZ80 does
#if 0
dzx7t_load_bits:
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
#endif
}
