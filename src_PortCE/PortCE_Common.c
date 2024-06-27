/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"

uint8_t simulated_ram[16777216];

fp64 getDecimalTime(void) {
	struct timespec tp;
	if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
		nano64_t nanoTime = tp.tv_sec * 1000000000 + tp.tv_nsec;
		return NANO_TO_SECONDS(nanoTime);
	} else {
		perror("clock_gettime");
		return 0.0;
	}
}

nano64_t getNanoTime(void) {
	struct timespec tp;
	if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
		nano64_t nanoTime = tp.tv_sec * 1000000000 + tp.tv_nsec;
		return nanoTime;
	} else {
		perror("clock_gettime");
		return 0;
	}
}