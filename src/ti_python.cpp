#include <ti/python.h>

void os_MSDGetMaxLUN(
    __attribute__((__unused__)) uint8_t *value
) {
    return;
}

void os_MSDReset(
    __attribute__((__unused__)) uint8_t value
) {
    return;
}

int8_t os_MSDInquiry(
    __attribute__((__unused__)) uint8_t lun,
    __attribute__((__unused__)) uint8_t *inquiry
) {
    return 5;
}

void os_MSDTestUnitReady(
    __attribute__((__unused__)) uint8_t lun,
    __attribute__((__unused__)) uint8_t *status
) {
    return;
}

int8_t os_MSDReadCapacity(
    __attribute__((__unused__)) uint8_t lun,
    __attribute__((__unused__)) uint32_t data[2]
) {
    return 5;
}

int8_t os_MSDRead(
    __attribute__((__unused__)) uint8_t lun,
    __attribute__((__unused__)) uint8_t blockCount,
    __attribute__((__unused__)) uint32_t lba,
    __attribute__((__unused__)) uint24_t blockSize,
    __attribute__((__unused__)) void *buffer
) {
    return 5;
}

int8_t os_MSDWrite(
    __attribute__((__unused__)) uint8_t lun,
    __attribute__((__unused__)) uint8_t blockCount,
    __attribute__((__unused__)) uint32_t lba,
    __attribute__((__unused__)) uint24_t blockSize,
    __attribute__((__unused__)) void *buffer
) {
    return 5;
}
