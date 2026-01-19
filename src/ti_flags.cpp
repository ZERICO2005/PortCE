#include <ti/flags.h>
#include "os_flags.hpp"
#include <PortCE.h>

static uint8_t get_lo(uint16_t pattern) {
    return static_cast<uint8_t>(pattern & 0xFF);
}

static uint8_t get_lo(int16_t pattern) {
    return get_lo(static_cast<uint16_t>(pattern));
}

static uint8_t get_hi_u(uint16_t pattern) {
    return static_cast<uint8_t>((pattern >> 8) & 0xFF);
}

static int8_t get_hi_s(int16_t pattern) {
    return static_cast<int8_t>((pattern >> 8) & 0xFF);
}

void os_SetFlagByte(int offset, uint8_t set) {
    *get_flag_pointer(offset) |= set;
}

uint8_t os_GetFlagByte(int offset) {
    return *get_flag_pointer(offset);
}

ti_int os_TestFlagBits(uint16_t offset_pattern) {
    uint8_t val = *get_flag_pointer(get_hi_u(offset_pattern));
    return (val & get_lo(offset_pattern)) ? 1 : 0;
}

void os_SetFlagBits(int16_t offset_pattern) {
    *get_flag_pointer(get_hi_s(offset_pattern)) |= get_lo(offset_pattern);
}

void os_ResetFlagBits(int16_t offset_pattern) {
    *get_flag_pointer(get_hi_s(offset_pattern)) &= ~get_lo(offset_pattern);
}

bool os_TestFlagBitsFast(uint16_t offset_pattern) {
    uint8_t val = *get_flag_pointer(get_hi_u(offset_pattern));
    return (val & get_lo(offset_pattern));
}
