#include <cstdint>
#include <cassert>
#include <cstring>
#include <array>
#include "../PortCE_memory.hpp"

static constexpr size_t FULL_ADDRESS_SIZE = 16777216;

static constexpr uint24_t BASE_tice = 0xD00000;
static constexpr uint24_t BASE_vram = 0xD40000;
static constexpr uint24_t BASE_mmio = 0xE00000;

static constexpr uint24_t SIZE_tice = BASE_vram - BASE_tice;
static constexpr uint24_t SIZE_vram = 320 * 240 * 2;
static constexpr uint24_t SIZE_mmio = FULL_ADDRESS_SIZE - SIZE_mmio;

static uint8_t MEM_tice[SIZE_tice];
static uint8_t MEM_vram[SIZE_vram];
static uint8_t MEM_mmio[SIZE_mmio];

struct ADDR {
    uint8_t * ptr;
    size_t base;
    size_t length;
    bool offset_in_range(size_t offset) const {
        if (offset < base) {
            return false;
        }
        if (offset >= base + length) {
            return false;
        }
        return true;
    }
    void* get_ptr(size_t offset) const {
        if (!offset_in_range(offset)) {
            return nullptr;
        }
        return reinterpret_cast<void*>(ptr + (offset - base));
    }
    bool ptr_in_range(const void * input) const {
        const uint8_t* addr = static_cast<const uint8_t*>(input);
        if (addr < ptr) {
            return false;
        }
        if (addr >= ptr + length) {
            return false;
        }
        return true;
    }
    uint24_t get_offset(const void* pointer) const {
        assert(ptr_in_range(pointer));
        const uint8_t* addr = static_cast<const uint8_t*>(pointer);
        return static_cast<uint24_t>(addr - ptr);
    }
};

static ADDR ADDR_tice = {MEM_tice, BASE_tice, SIZE_tice};
static ADDR ADDR_vram = {MEM_vram, BASE_vram, SIZE_vram};
static ADDR ADDR_mmio = {MEM_mmio, BASE_mmio, SIZE_mmio};

static std::array<ADDR, 3> ADDR_list = {
    ADDR_tice,
    ADDR_vram,
    ADDR_mmio,
};

void * RAM_ADDRESS(uint24_t offset) {
    if (offset == 0) {
        return nullptr;
    }
    for (size_t i = 0; i < ADDR_list.size(); i++) {
        if (ADDR_list[i].offset_in_range(offset)) {
            return ADDR_list[i].get_ptr(offset);
        }
    }
    // if (ADDR_tice.offset_in_range(offset)) { return ADDR_tice.get_ptr(offset); }
    // if (ADDR_vram.offset_in_range(offset)) { return ADDR_vram.get_ptr(offset); }
    // if (ADDR_mmio.offset_in_range(offset)) { return ADDR_mmio.get_ptr(offset); }
    return nullptr;
}

uint24_t RAM_OFFSET(const void * ptr) {
    if (ptr == nullptr) {
        return 0;
    }
    for (size_t i = 0; i < ADDR_list.size(); i++) {
        if (ADDR_list[i].offset_in_range(ptr)) {
            return ADDR_list[i].get_offset(ptr);
        }
    }
    // if (ADDR_tice.ptr_in_range(ptr)) { return ADDR_tice.get_offset(ptr); }
    // if (ADDR_vram.ptr_in_range(ptr)) { return ADDR_vram.get_offset(ptr); }
    // if (ADDR_mmio.ptr_in_range(ptr)) { return ADDR_mmio.get_offset(ptr); }
    return 0;
}

const void* CONST_RAM_ADDRESS(const uint24_t address) {
    return static_cast<const void*>(RAM_OFFSET(address));
}

bool PortCE_memory_init(void) {
    PortCE_memory_clear_all();
    return true;
}

void PortCE_memory_destroy(void) {
    return;
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

/**
 * @brief Reads from PortCE memory
 * @note Reads zeros when out of range
 */
void PortCE_memory_read(void * dst, uint24_t offset, size_t size) {
    if (!is_valid_mem_param(offset, size)) {
        return;
    }
    if (dst == nullptr) {
        return;
    }

    // default values
    memset(dst, 0, size);
    if (size == 0) {
        return;
    }
    const size_t start = static_cast<size_t>(offset);
    const size_t end = start + size;
    uint8_t *out = static_cast<uint8_t*>(dst);
    for (size_t i = 0; i < ADDR_list.size(); i++) {
        const ADDR &region = ADDR_list[i];
        const size_t region_start = region.base;
        const size_t region_end = region.base + region.length;
        if (end <= region_start || start >= region_end) {
            continue;
        }
        const size_t copy_start = (start > region_start) ? start : region_start;
        const size_t copy_end = (end < region_end) ? end : region_end;
        const size_t copy_len = copy_end - copy_start;
        const size_t dst_offset = copy_start - start;
        const size_t src_offset = copy_start - region_start;
        memcpy(out + dst_offset, region.ptr + src_offset, copy_len);
    }
}

/**
 * @brief Writes to PortCE memory
 */
void PortCE_memory_write(const void * src, uint24_t offset, size_t size) {
    if (!is_valid_mem_param(offset, size)) {
        return;
    }
    if (src == nullptr) {
        return;
    }
    const size_t start = static_cast<size_t>(offset);
    const size_t end = start + size;
    const uint8_t *in = static_cast<const uint8_t*>(src);
    for (size_t i = 0; i < ADDR_list.size(); i++) {
        const ADDR &region = ADDR_list[i];
        const size_t region_start = region.base;
        const size_t region_end = region.base + region.length;
        if (end <= region_start || start >= region_end) {
            continue;
        }
        const size_t copy_start = (start > region_start) ? start : region_start;
        const size_t copy_end = (end < region_end) ? end : region_end;
        const size_t copy_len = copy_end - copy_start;
        const size_t src_offset = copy_start - start;
        const size_t dst_offset = copy_start - region_start;
        memcpy(region.ptr + dst_offset, in + src_offset, copy_len);
    }
}

/**
 * @brief zeros a memory region
 */
void PortCE_memory_memset(uint24_t offset, uint8_t value, size_t size) {
    if (!is_valid_mem_param(offset, size)) {
        return;
    }
    const size_t start = static_cast<size_t>(offset);
    const size_t end = start + size;
    for (size_t i = 0; i < ADDR_list.size(); i++) {
        const ADDR &region = ADDR_list[i];
        const size_t region_start = region.base;
        const size_t region_end = region.base + region.length;
        if (end <= region_start || start >= region_end) {
            continue;
        }
        const size_t clear_start = (start > region_start) ? start : region_start;
        const size_t clear_end = (end < region_end) ? end : region_end;
        const size_t clear_len = clear_end - clear_start;
        const size_t dst_offset = clear_start - region_start;
        memset(region.ptr + dst_offset, 0, clear_len);
    }
}

/**
 * @brief zeros all memory regions
 */
void PortCE_mem_clear_all() {
    for (size_t i = 0; i < ADDR_list.size(); i++) {
        memset(ADDR_list[i].ptr, 0, ADDR_list[i].length);
    }
}
