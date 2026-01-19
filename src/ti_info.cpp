#include <ti/info.h>
#include <cstring>

const system_info_t *os_GetSystemInfo(void) {
    static system_info_t info;
    memset(&info, 0, sizeof(system_info_t));

    /* size = bytes after this field */
    info.size = sizeof(system_info_t) - sizeof(size_t);

    /* Hardware (TI-84 Plus CE) */
    info.hardwareVersion = 7;
    info.hardwareType    = 0;
    info.hardwareType2   = 9;

    /* OS version: 5.4.0.0034 */
    info.osMajorVersion    = 5;
    info.osMinorVersion    = 4;
    info.osRevisionVersion = 0;
    info.osBuildVersion    = 34;

    /* Boot version: 5.3.6.0017 */
    info.bootMajorVersion    = 5;
    info.bootMinorVersion    = 3;
    info.bootRevisionVersion = 6;
    info.bootBuildVersion    = 17;

    /* Unknown constant block */
    static const uint8_t ce_unknown[10] = {
        0x40, 0x01, 0x00, 0xF0, 0x00,
        0x00, 0x10, 0x30, 0x00, 0x00
    };

    memcpy(info.unknown, ce_unknown, sizeof(ce_unknown));

    /* Calculator ID (certificate-derived) unknown here */
    memset(info.calcid, 0, sizeof(info.calcid));

    /* Device name prefix */
    info.ti[0] = 'T';
    info.ti[1] = 'I';

    /* Language: English */
    info.language = 0x0109;
    return &info;
}

uint8_t boot_GetBootMajorVer(void) {
    return os_GetSystemInfo()->bootMajorVersion;
}

uint8_t boot_GetBootMinorVer(void) {
    return os_GetSystemInfo()->bootMinorVersion;
}

uint8_t boot_GetHardwareVer(void) {
    return os_GetSystemInfo()->hardwareVersion;
}
