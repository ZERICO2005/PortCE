#include "PortCE_Common.h"
#include <sys/power.h>
#include "os_flags.hpp"

void os_DisableAPD(void) {
    iy_flag_res(2, +0x08);
}

void os_EnableAPD(void) {
    iy_flag_set(2, +0x08);
}

void boot_TurnOn(void) {
    return;
}

void boot_TurnOff(void) {
    return;
}

void boot_Set6MHzMode(void) {
    set_clockspeed(6.0e6);
}

void boot_Set48MHzMode(void) {
    set_clockspeed(48.0e6);
}

void boot_Set6MHzModeI(void) {
    set_clockspeed(6.0e6);
}

void boot_Set48MHzModeI(void) {
    set_clockspeed(48.0e6);
}

uint8_t boot_GetBatteryStatus(void) {
    /* value between 0 (empty) to 4 (full) */
    return 3;
}

uint8_t boot_BatteryCharging(void) {
    return 0;
}
