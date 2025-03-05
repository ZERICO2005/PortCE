#ifndef PORTCE_INTERNAL_H
#define PORTCE_INTERNAL_H

#ifndef PORTCE_INTERNAL
#define PORTCE_INTERNAL
#endif

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int TiOS_vprintf(
	const char *__restrict format, va_list va
) __attribute__((format(__printf__, 1, 0)));

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_INTERNAL_H */
