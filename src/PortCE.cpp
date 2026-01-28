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
#include "PortCE_memory.hpp"

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

void PortCE_initialize(const char* window_title) {
    bool memory_status = PortCE_memory_init();
    if (!memory_status) {
        perror("PortCE_memory_init failed");
        exit(EXIT_FAILURE);
    }

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
    PortCE_memory_destroy();
    PortCE_init_flag = false;
}
