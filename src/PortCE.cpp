/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"
#include "PortCE_Render.h"
#include "PortCE_SPI.h"

#include <cstdlib>
#include <cstdio>
#include <PortCE.h>
#include "PortCE_assert.h"

static bool PortCE_init_flag = false;

bool is_PortCE_initialized(void) {
    return PortCE_init_flag;
}

void assert_PortCE_initialized(void) {
    if (!is_PortCE_initialized()) {
        fprintf(stderr,
            "FATAL ERROR: PortCE is not initialized.\n"
            "You need to add PortCE_initialize() and PortCE_terminate() to your code.\n"
        );
        exit(EXIT_FAILURE);
    }
}

void* RAM_ADDRESS(const uint24_t address) {
    if (address == 0) {
        return nullptr;
    }
    // if (address >= 0xE00000 && update_ram == false) {
    //     PortCE_update_registers();
    //     return &simulated_ram[address];
    // }
    return &simulated_ram[address];
}

uint24_t RAM_OFFSET(const void* const ptr) {
    if (ptr == nullptr) {
        return 0;
    }
    const ptrdiff_t offset = ((const uint8_t*)ptr - simulated_ram);
    if (offset < 0 || (size_t)offset >= sizeof(simulated_ram)) {
        return 0;
    }
    return (uint24_t)offset;
}

void PortCE_initialize(const char* window_title) {
    // memset(simulated_ram, 0, sizeof(simulated_ram));
    reset_ti84ce_registers();
    PortCE_reset_SPI_state(true);

    // import_config_file();
    PortCE_Config config = {2, false, false, false, false};
    initLCDcontroller(window_title, &config);
    PortCE_init_flag = true;
}

void PortCE_terminate(void) {
    terminateLCDcontroller();
    PortCE_terminate_fileioc();
    // export_config_file();
    PortCE_init_flag = false;
}
