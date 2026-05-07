#include <cstdint>
#include "PortCE_Common.h"
#include "PortCE_memory.hpp"
#include "ti84pceg.hpp"
#include <ti/screen.h>
#include <sys/basicusb.h>
#include <sys/timers.h>
#include <PortCE.h>
#include "os_flags.hpp"

static uint8_t exit_flash_wait_states;

static constexpr uint8_t c_program_wait_states = 3;

void PortCE_crt0_init(void) {
    /* __start: */
    os_RunIndicOff();
    boot_USBDisableTimers();
    /* di */

    iy_flag_res(1, +0x0D); // no text buffer
    iy_flag_res(3, +0x4A); // use first shadow buffer
    iy_flag_set(5, +0x4C); // only display

    exit_flash_wait_states = flash_wait_states;
    flash_wait_states = c_program_wait_states;

/* HAS_CLOCK */
#if 1
    timer_Disable(1);
    timer_Set(1, UINT32_C(0x00000000));
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);
#endif

}

void PortCE_crt0_fini(void) {
    flash_wait_states = exit_flash_wait_states;
    boot_USBResetTimers();
    *static_cast<uint8_t*>(RAM_ADDRESS(0xE00305)) = 1; // clear on interrupt
    iy_flag_res(4, +0x09); // Interrupt,(iy+onFlags)
    iy_flag_set(0, +0x03); // graphDraw,(iy+graphFlags)
    iy_flag_set(1, +0x0D); // use text buffer
    iy_flag_res(3, +0x4A); // use first shadow buffer
    iy_flag_res(5, +0x4C); // use shadow buffer
    os_ClrLCDFull();
    os_HomeUp();
    os_DrawStatusBar();
}
