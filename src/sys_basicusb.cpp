#include <sys/basicusb.h>

bool boot_USBBusPowered(void) {
    return false;
}

bool boot_USBSelfPowered(void) {
    return false;
}

void boot_USBResetChip(void) {
    return;
}

void boot_USBDisableTimers(void) {
    return;
}

void boot_USBEnableTimers(void) {
    return;
}

void boot_USBResetTimers(void) {
    return;
}

int8_t os_USBGetRequestStatus(void) {
    return 0;
}
