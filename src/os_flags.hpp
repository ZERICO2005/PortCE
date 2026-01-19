#ifndef OS_FLAGS_HPP
#define OS_FLAGS_HPP

#include <cstdint>
#include <cassert>

inline uint8_t* get_flag_pointer(int offset = 0) {
    assert(offset >= INT8_MIN && offset <= INT8_MAX);
    constexpr uint24_t base_addr = 0xD00080;
    uint24_t addr = static_cast<uint24_t>(base_addr + offset);
    return static_cast<uint8_t*>(RAM_ADDRESS(addr));
}

inline void iy_flag_set(uint8_t bit, int offset) {
    *get_flag_pointer(offset) |= static_cast<uint8_t>(1 << bit);
}

inline void iy_flag_res(uint8_t bit, int offset) {
    *get_flag_pointer(offset) &= static_cast<uint8_t>(~(1 << bit));
}

inline bool iy_flag_bit(uint8_t bit, int offset) {
    return (*get_flag_pointer(offset) & static_cast<uint8_t>(1 << bit));
}

#endif /* OS_FLAGS_HPP */
