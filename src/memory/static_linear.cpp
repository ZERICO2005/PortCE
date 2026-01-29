#include "../PortCE_memory.hpp"
#include <cstring>
#include <cassert>

static constexpr size_t FULL_ADDRESS_SIZE = 16777216;

uint8_t PortCE_static_linear_memory[FULL_ADDRESS_SIZE];

bool PortCE_memory_init(void) {
    PortCE_memory_clear_all();
    return true;
}

void PortCE_memory_destroy(void) {
    return;
}

void *RAM_ADDRESS(const uint24_t address) {
    if (address == 0) {
        return nullptr;
    }
    return &PortCE_static_linear_memory[address];
}

const void *CONST_RAM_ADDRESS(const uint24_t address) {
    return static_cast<const void*>(RAM_ADDRESS(address));
}

uint24_t RAM_OFFSET(const void* const ptr) {
    if (ptr == nullptr) {
        return 0;
    }
    const ptrdiff_t offset = static_cast<const uint8_t*>(ptr) - PortCE_static_linear_memory;
    if (offset <= 0) {
        // points to NULL or behind PortCE memory
        return 0;
    }
    if (static_cast<size_t>(offset) >= sizeof(PortCE_static_linear_memory)) {
        // points ahead of PortCE memory
        return 0;
    }
    return static_cast<uint24_t>(offset);
}

__attribute__((__unused__))
static bool is_valid_mem_param(uint24_t offset, size_t size) {
    if (size == 0) {
        // invalid
        return false;
    }
    if (offset == 0) {
        // points to NULL
        return false;
    }
    if (size >= FULL_ADDRESS_SIZE) {
        // too large
        return false;
    }
    if (offset + size > FULL_ADDRESS_SIZE) {
        // out of range
        return false;
    }
    return true;
}

void PortCE_memory_read(void * dst, uint24_t src_offset, size_t size) {
    assert(is_valid_mem_param(src_offset, size));
    assert(dst != nullptr);
    const void * src = CONST_RAM_ADDRESS(src_offset);
    assert(src != nullptr);
    memcpy(dst, src, size);
}

void PortCE_memory_write(uint24_t dst_offset, const void * src, size_t size) {
    assert(is_valid_mem_param(dst_offset, size));
    assert(src != nullptr);
    void * dst = RAM_ADDRESS(dst_offset);
    assert(dst != nullptr);
    memcpy(dst, src, size);
}

void PortCE_memory_memset(uint24_t offset, uint8_t value, size_t size) {
    assert(is_valid_mem_param(offset, size));
    void * dst = RAM_ADDRESS(offset);
    assert(dst != nullptr);
    memset(dst, value, size);
}

void PortCE_memory_clear_all(void) {
    memset(PortCE_static_linear_memory, 0, sizeof(PortCE_static_linear_memory));
}
