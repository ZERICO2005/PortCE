#ifndef PORTCE_LIBC_H
#define PORTCE_LIBC_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// memory functions
//------------------------------------------------------------------------------

#if 1

/**
 * @brief simulates the LDIR instruction on the eZ80. This allows dst and src
 * to overlap.
 */
void* eZ80_memcpy(void *dst, const void *src, size_t count);

// void* eZ80_malloc(size_t size);
// void* eZ80_free(void *ptr);
// void* eZ80_calloc(size_t count, size_t size);
// void* eZ80_realloc(size_t count, size_t size);
// void* eZ80_memset(void *dst, int ch, size_t count);

//------------------------------------------------------------------------------
// printf
//------------------------------------------------------------------------------

/**
 * @brief prints to TiOS and the console. Using <inttypes.h> is recommended
 */
int eZ80_printf(const char *__restrict format, ...) __attribute__((format(__printf__, 1, 2)));

//------------------------------------------------------------------------------
// time and date
//------------------------------------------------------------------------------

#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_LIBC_H */
