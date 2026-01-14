#include <stdint.h>
#include <algorithm>

typedef union reg {
    uint16_t full;
    struct {
        uint8_t low;
        uint8_t high;
    };
} reg;

static uint8_t a;
static bool carry;
static reg reg_bc;
static reg reg_de;
static reg reg_hl;
static reg reg_sp;

#define bc reg_bc.full
#define c  reg_bc.low
#define b  reg_bc.high

#define de reg_de.full
#define e  reg_de.low
#define d  reg_de.high

#define hl reg_hl.full
#define l  reg_hl.low
#define h  reg_hl.high

#define sp  reg_sp.full
#define spl reg_sp.low
#define sph reg_sp.high

static void rla() {
    bool next = (a & 0x80);
    a <<= 1;
    if (carry) { a |= 0x01; }
    carry = next;
}

static void rra() {
    bool next = (a & 0x01);
    a >>= 1;
    if (carry) { a |= 0x80; }
    carry = next;
}

static void add_hl_hl_sis() {
    carry = (hl & 0x8000);
    hl <<= 1;
}

uint16_t internal_gfz_Darken(uint16_t color, uint8_t amount);

uint16_t internal_gfz_Darken(uint16_t color, uint8_t amount) {
    l = amount;
    bc = color;
    sp = color;
    a = c;
    c = l;
    a &= 31;
    h = a;
    hl = h * l;
    de = 128;
    hl += de; carry = false;
    l = h;
    h = d;
    std::swap(hl, sp);
    a = h;
    rra();
    a &= 62;
    b = a;
    add_hl_hl_sis();
    rla();
    hl <<= 1; carry = false;
    hl <<= 1; carry = false;
    rra();
    a = h;
    rla();
    a &= 63;
    h = a;
    l = c;
    hl = h * l;
    hl += de;
    l = h;
    bc = b * c;
    ++b;
    b >>= 1;
    hl <<= 1;
    hl <<= 1;
    h = b;
    hl <<= 1;
    hl <<= 1;
    if (!(l & (1 << 4))) {
        goto out;
    }
    h |=  (1 << 7);
    l &= ~(1 << 4);
out:
    bc = sp;
    hl += bc;
    return hl;
}
