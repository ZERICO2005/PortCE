/**
 * @file
 *
 * @author "zerico2005"
 */

#ifndef PORTCE_H
#define PORTCE_H

#include <stdint.h>

#ifndef _EZ80
	// For debugging purposes
	#include <stdio.h>
#endif

#include <PortCE_type.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// pointer macros
//------------------------------------------------------------------------------

#ifdef _EZ80

	/**
	 * @brief Access pointers through this macro
	 */
	#define RAM_ADDRESS(x) ((void*)(x))

	/**
	 * @brief Access pointers through this macro at compile time
	 */
	#define CONST_ADDRESS(x) ((void*)x)
	
	/**
	 * @brief Calculate pointer offsets from this macro
	 */
	#define RAM_OFFSET(ptr) ((uint24_t)(ptr))

	#define PortCE_initialize(...)

	#define PortCE_terminate()

	#define PortCE_pace_frame(...)

	#define PortCE_new_frame()

	#define PortCE_update_registers() 0

#else

	extern uint8_t simulated_ram[16777216];

	void* RAM_ADDRESS(uint24_t address);

	#define CONST_ADDRESS(x) ((void*)simulated_ram[(x)])

	uint24_t RAM_OFFSET(const void* ptr);

	/**
	 * @brief Initializes the PortCE screen, keyboard, timers, etc.
	 * 
	 * @param window_title your programs name (Can be NULL)
	 */
	void PortCE_initialize(const char* window_title);

	/**
	 * @brief Destroys the PortCE window.
	 */
	void PortCE_terminate(void);

	/**
	 * @brief Manually refreshes the screen.
	 */
	void PortCE_new_frame(void);

	/**
	 * @brief Manually refreshes the screen.
	 */
	void PortCE_pace_frame(float frame_rate);

	/**
	 * @brief Used to update registers
	 * 
	 * @return This function always returns 0
	 */
	ti_int PortCE_update_registers(void);
	
#endif

/* Clock */

	#ifndef _EZ80
		#define TI_CLOCKS_PER_SEC 32768
		typedef ti_long ti_clock_t;
		ti_clock_t ti_clock(void);
	#else
		#define TI_CLOCKS_PER_SEC CLOCKS_PER_SEC
		#define ti_clock_t clock_t
		#define ti_clock() clock()
	#endif

#ifdef __cplusplus
}
#endif

#if !defined(_EZ80) && !defined(PORTCE_INTERNAL)
	#include <PortCE_libc.h>
	#undef memcpy
	#define memcpy eZ80_memcpy
	#if 0
		#undef printf
		#define printf eZ80_printf
	#endif
#endif

#endif /* PORTCE_H */
