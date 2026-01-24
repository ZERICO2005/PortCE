/**
 * @file
 *
 * @author "zerico2005"
 */

// #include "PortCE_Common.h"
#include <stdint.h>
#include "PortCE_Common.h"

uint8_t simulated_ram[16777216];
static double Ti84CE_Clockspeed = 8.0e6;

void set_clockspeed(double clock_speed) {
    Ti84CE_Clockspeed = clock_speed;
}

double get_clockspeed() {
    return Ti84CE_Clockspeed;
}
