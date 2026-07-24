#include <PortCE.h>

static uint32_t ce_random_state[4] = {
    UINT32_C(0x0D0E0F10),
    UINT32_C(0x090A0B0C),
    UINT32_C(0x05060708),
    UINT32_C(0x01020304),
};

static uint32_t xsadd(uint32_t state[4], bool& carry_out) {
	uint32_t s0, s1, s2, t0, t1, t2, t3, z, result;
	t0 = state[0];
	state[0] = state[1];
    state[1] = state[2];
    state[2] = state[3];
	s0 = t0 << 19;
	t1 = t0 ^ s0;
	s1 = t1 >> 21;
	t2 = t1 ^ s1;
	z = state[3];
	s2 = z << 8;
	t3 = t2 ^ s2;
	state[3] = t3;
    carry_out = __builtin_add_overflow(t3, state[2], &result);
	return result;
}

static uint32_t ce_random_impl(bool& carry_out) {
    return xsadd(ce_random_state, carry_out);
}

uint32_t ce_random(void) {
    bool carry_out;
    return ce_random_impl(carry_out);
}

void ce_srandom(uint32_t seed) {
    ce_random_state[0] = seed;
    uint8_t B = 12;
    uint8_t *HL = &reinterpret_cast<uint8_t*>(ce_random_state)[3];
    do {
        ++HL;
        *HL = B;
    } while(--B != 0);
}

int24_t ce_rand(void) {
    bool carry_out;
    uint32_t result = ce_random_impl(carry_out);
    result -= (0x800000 + (carry_out ? 1 : 0));
    result &= 0xFFFFFF;
    if (result & 0x800000) {
        result += 0x800000;
        result &= 0xFFFFFF;
    }
    return static_cast<int24_t>(result);
}

void ce_srand(uint24_t seed) {
    ce_srandom(seed);
}
