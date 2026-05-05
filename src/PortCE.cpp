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
#include "ti84pceg.hpp"
#include "PortCE_quit.hpp"

static bool PortCE_init_flag = false;
static bool PortCE_terminate_was_called = false;

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

void PortCE_terminate(void) {
    if (!PortCE_init_flag) {
        fprintf(stderr, "Warning: PortCE_terminate() was called but PortCE is not initialized.\n");
        return;
    }
    PortCE_terminate_was_called = true;

    terminateLCDcontroller();
    PortCE_terminate_fileioc();
    // export_config_file();
    PortCE_memory_destroy();
    PortCE_init_flag = false;
}

[[noreturn]] void PortCE_quit(int exit_code) {
    PortCE_terminate();
    exit(exit_code);
}

static void PortCE_atexit(void) {
    if (!PortCE_terminate_was_called) {
        fprintf(stderr, "Warning: PortCE_terminate() was not called.\n");
    }
}

static void PortCE_register_atexit(void) {
    static bool successfully_registered = false;
    if (successfully_registered) {
        return;
    }
    if (std::atexit(PortCE_atexit)) {
        fprintf(stderr, "Failed to register PortCE_atexit.\n");
        return;
    }
    successfully_registered = true;
}

void PortCE_initialize(const char* window_title) {
    if (PortCE_init_flag) {
        fprintf(stderr, "Warning: PortCE is already initialized.\n");
        return;
    }
    PortCE_terminate_was_called = false;
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
    PortCE_register_atexit();
}
