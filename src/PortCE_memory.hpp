#ifndef PORTCE_MEMORY_HPP
#define PORTCE_MEMORY_HPP

#include <PortCE.h>

/**
 * @brief initializes PortCE memory
 * @returns true on success
 */
bool PortCE_memory_init(void);

/**
 * @brief cleans up PortCE memory
 */
void PortCE_memory_destroy(void);

/**
 * @brief Reads from PortCE memory
 * @note Reads zeros when out of range
 */
void PortCE_memory_read(void * dst, uint24_t src_offset, size_t size);

/**
 * @brief Writes to PortCE memory
 */
void PortCE_memory_write(uint24_t dst_offset, const void * src, size_t size);

/**
 * @brief sets a memory region to a value
 */
void PortCE_memory_memset(uint24_t offset, uint8_t value, size_t size);

/**
 * @brief zeros a memory region
 */
inline void PortCE_memory_clear(uint24_t offset, size_t size) {
    PortCE_memory_memset(offset, 0, size);
}

/**
 * @brief zeros all memory regions
 */
void PortCE_memory_clear_all(void);

#endif /* PORTCE_MEMORY_HPP */
