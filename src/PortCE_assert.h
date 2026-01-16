#ifndef PORTCE_ASSERT_H
#define PORTCE_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool is_PortCE_initialized(void);

void assert_PortCE_initialized(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_ASSERT_H */
