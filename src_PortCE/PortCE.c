/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE.h"

#include "PortCE_Common.h"
#include "PortCE_Render.h"
#include "PortCE_SPI.h"

void* RAM_ADDRESS(const uint24_t address) {
	// if (address >= 0xE00000 && update_ram == false) {
	// 	PortCE_update_registers();
	// 	return &simulated_ram[address];
	// }
	return &simulated_ram[address];
}

uint24_t RAM_OFFSET(const void* const ptr) {
	const ptrdiff_t offset = ((const uint8_t*)ptr - simulated_ram);
	if (offset < 0 || (size_t)offset >= sizeof(simulated_ram)) {
		return 0;
	}
	return offset;
}

/* Function Declarations */

	void PortCE_terminate_fileioc(void);
	
	void reset_ti84ce_registers(void);

void PortCE_initialize(const char* window_title) {
	// memset(simulated_ram, 0, sizeof(simulated_ram));
	reset_ti84ce_registers();
	PortCE_reset_SPI_state(true);

	// import_config_file();
	PortCE_Config config = {2, false, false, false, false};
	initLCDcontroller(window_title, &config);
	
}

void PortCE_terminate(void) {
	terminateLCDcontroller();
	PortCE_terminate_fileioc();
	// export_config_file();
}
