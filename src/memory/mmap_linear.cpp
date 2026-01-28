#include "../PortCE_memory.hpp"
#include <sys/mman.h>
#include <cstring>
#include <cstdint>

static constexpr size_t FULL_ADDRESS_SIZE = 16777216;
static constexpr uintptr_t MINIMUM_ADDRESS = 0xD00000;
static constexpr uintptr_t MAXIMUM_ADDRESS = 0xFFFFFF;
static constexpr size_t MEMORY_SIZE = MAXIMUM_ADDRESS - MINIMUM_ADDRESS + 1;

static void * mmap_ptr = nullptr;
static constexpr size_t mmap_size = MEMORY_SIZE;

bool PortCE_memory_init(void) {
    void * mmap_ptr = mmap(
        reinterpret_cast<void*>(MINIMUM_ADDRESS),
        mmap_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
        -1,
        0
    );

    if (mmap_ptr == MAP_FAILED || mmap_ptr == nullptr) {
        return false;
    }

    return true;
}

void PortCE_memory_destroy(void) {
    if (munmap(mmap_ptr, mmap_size)) {
        perror("PortCE_memory_destroy --> munmap");
    }
}

void *RAM_ADDRESS(const uint24_t address) {
    if (address == 0) {
        return nullptr;
    }
    return reinterpret_cast<void*>(static_cast<uintptr_t>(address));
}

const void* CONST_RAM_ADDRESS(const uint24_t address) {
    return static_cast<const void*>(RAM_ADDRESS(address));
}

uint24_t RAM_OFFSET(const void* const ptr) {
    if (ptr == nullptr) {
        return 0;
    }
    const ptrdiff_t offset = ((const uint8_t*)ptr - PortCE_linear_address);
    if (offset <= 0) {
        return 0;
    }
    if (static_cast<size_t>(offset) >= sizeof(PortCE_linear_address)) {
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
    memset(RAM_ADDRESS(MINIMUM_ADDRESS), 0, MEMORY_SIZE);
}
