/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"

uint8_t simulated_ram[16777216];

SPI_State PortCE_SPI_State;

nano64_t getNanoTime(void) {
	struct timespec tp;
	if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
		nano64_t nanoTime = (nano64_t)tp.tv_sec * (nano64_t)1000000000 + (nano64_t)tp.tv_nsec;
		return nanoTime;
	} else {
		perror("clock_gettime");
		return 0;
	}
}

fp64 getDecimalTime(void) {
	return NANO_TO_SECONDS(getNanoTime());
}
