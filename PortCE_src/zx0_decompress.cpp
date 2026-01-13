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
static reg reg_iy;

#define bc reg_bc.full
#define c  reg_bc.low
#define b  reg_bc.high

#define iy  reg_iy.full
#define iyl reg_iy.low
#define iyh reg_iy.high

#define call(cond, func) if ((cond)) { func(); }

#define jr(cond, label) if ((cond)) { goto label; }

#define ret(cond) if ((cond)) { return; }

static void scf() {
    carry = true;
}

static void add_a_a() {
    carry = (a & 0x80);
    a <<= 1;
    zero = (a == 0);
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

static void rla() {
    bool next = (a & 0x80);
    a <<= 1;
    if (carry) { a |= 0x01; }
    carry = next;
}

//------------------------------------------------------------------------------
// ZX0 decoder by Einar Saukas & introspec
// "Turbo" version
//------------------------------------------------------------------------------

static void zx0_elias(void) {
dzx0t_elias:
    // interlaced Elias gamma coding
    add_a_a();
    rl(c);
    add_a_a();
    jr(!carry, dzx0t_elias);
    ret(!zero);
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
    ret(carry);
    add_a_a();
    rl(c);
    add_a_a();
    ret(carry);
    add_a_a();
    rl(c);
    add_a_a();
    ret(carry);
    add_a_a();
    rl(c);
    add_a_a();
    ret(carry);
dzx0t_elias_loop:
    add_a_a();
    rl(c);
    rl(b);
    add_a_a();
    jr(!carry, dzx0t_elias_loop);
    ret(!zero)
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
    jr(!carry, dzx0t_elias_loop);
    return;
}

static void set_offset(intptr_t& offset, uint16_t value) {
    constexpr intptr_t mask = 0xFFFF;
    offset = ~mask;
    offset |= value;
}

void zx0_Decompress(void *arg_dst, const void *arg_src) {
    dst = (uint8_t*)arg_dst;
    src = (const uint8_t*)arg_src;
    if (arg_dst == nullptr || arg_src == nullptr) {
        fprintf(stderr, "zx0_Decompress: nullptr arguments: dst(%p) src(%p)\n", dst, src);
        return;
    }

    intptr_t offset = -1;

    // preserve default offset 1
    iy = -1;
    bc = 1;
    scf();
    goto dzx0t_start;

dzx0t_new_offset:
    // prepare negative offset
    --bc;
    --bc;
    add_a_a();
    jr(!zero, dzx0t_new_offset_skip);
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
dzx0t_new_offset_skip:
    // obtain offset MSB
    call(!carry, zx0_elias);
    inc(c);
    // check end marker
    ret(zero);
    // obtain offset LSB
    b = *src;
    ++src;
    // last offset bit becomes first length bit
    rr(c);
    rr(b);
    // preserve new offset
    iyl = b;
    iyh = c;
    // obtain length
    bc = 1;
    call(!carry, zx0_elias);
    ++bc;
dzx0t_copy:
    // preserve source
    // push    hl
    // dzx0t_last_offset:
    // restore offset
    // lea    hl, iy + 0
    // calculate destination - offset
    // add    hl, de
    set_offset(offset, iy);
    // copy from offset
    /* LDIR */ do {
        *dst = *(dst + offset);
        ++dst;
    } while (--bc != 0);
    // restore source
    // pop    hl
    // copy from literals or new offset?
    add_a_a();
    jr(carry, dzx0t_new_offset);
// dzx0t_literals:
    inc(c);        // obtain length
    add_a_a();
    jr(!zero, dzx0t_literals_skip);
dzx0t_start:
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
dzx0t_literals_skip:
    call(!carry, zx0_elias);
    // copy literals
    /* LDIR */ do {
        *dst = *src;
        ++dst;
        ++src;
    } while (--bc != 0);
    // copy from last offset or new offset?
    add_a_a();
    jr(carry, dzx0t_new_offset);
    inc(c);        // obtain length
    add_a_a();
    jr(!zero, dzx0t_last_offset_skip);
    // load another group of 8 bits
    a = *src;
    ++src;
    rla();
dzx0t_last_offset_skip:
    call(!carry, zx0_elias);
    goto dzx0t_copy;
}
