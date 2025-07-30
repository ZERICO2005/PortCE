#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "PortCE_internal.h"

#if 0

void* eZ80_memcpy(void *dst, const void *src, size_t count) {
	// simulates LDIR
	size_t BC = count;
	const uint8_t *HL = (const uint8_t*)src;
	uint8_t *DE = (uint8_t*)dst;
	while (BC --> 0) {
		*DE = *HL;
		HL++;
		DE++;
	}
	return dst;
}

int eZ80_printf(const char *__restrict format, ...) {
	va_list args;
	int ret;
	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);
	va_start(args, format);
	ret = TiOS_vprintf(format, args);
	va_end(args);

	return ret; // Return the result of vprintf
}

#endif
