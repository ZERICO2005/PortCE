#include "../PortCE_memory.hpp"
#include <cstring>

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

const void* CONST_RAM_ADDRESS(const uint24_t address) {
    return static_cast<const void*>(RAM_ADDRESS(address));
}

uint24_t RAM_OFFSET(const void* const ptr) {
    if (ptr == nullptr) {
        return 0;
    }
    const ptrdiff_t offset = ((const uint8_t*)ptr - PortCE_static_linear_memory);
    if (offset <= 0) {
        return 0;
    }
    if (static_cast<size_t>(offset) >= sizeof(PortCE_static_linear_memory)) {
        return 0;
    }
    return static_cast<uint24_t>(offset);
}

static bool is_valid_mem_param(uint24_t offset, size_t size) {
    if (size == 0) {
        return false;
    }
    if (offset == 0) {
        return false;
    }
    if (size >= FULL_ADDRESS_SIZE) {
        return false;
    }
    if (offset + size > FULL_ADDRESS_SIZE) {
        return false;
    }
    return true;
}

void PortCE_memory_read(void * dst, uint24_t offset, size_t size) {
    if (!is_valid_mem_param(offset, size)) {
        return;
    }
    if (dst == nullptr) {
        return;
    }
    const void * src = CONST_RAM_ADDRESS(offset);
    if (src == nullptr) {
        return;
    }
    memcpy(dst, src, size);
}

void PortCE_memory_write(const void * src, uint24_t offset, size_t size) {
    if (!is_valid_mem_param(offset, size)) {
        return;
    }
    if (src == nullptr) {
        return;
    }
    void * dst = RAM_ADDRESS(offset);
    if (dst == nullptr) {
        return;
    }
    memcpy(dst, src, size);
}

void PortCE_memory_memset(uint24_t offset, uint8_t value, size_t size) {
    if (!is_valid_mem_param(offset, size)) {
        return;
    }
    void * dst = RAM_ADDRESS(offset);
    if (dst == nullptr) {
        return;
    }
    memset(RAM_ADDRESS(offset), value, size);
}

void PortCE_memory_clear_all(void) {
    memset(PortCE_static_linear_memory, 0, sizeof(PortCE_static_linear_memory));
}
