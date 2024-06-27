/**
 * @file
 *
 * @author "zerico2005"
 */

#ifndef PORTCE_H
#define PORTCE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _EZ80
	typedef signed _BitInt(24) int24_t; /** @note Remember to account for integer promotion */
	typedef unsigned _BitInt(24) uint24_t; /** @note Remember to account for integer promotion */
	
	typedef signed _BitInt(48) int48_t;
	typedef unsigned _BitInt(48) uint48_t;
#endif

/* Integer types */
	typedef int16_t ti_short;
	typedef int24_t ti_int; /** @note Remember to account for integer promotion */
	typedef int32_t ti_long;
	typedef int64_t ti_long_long;

	typedef int16_t ti_signed_short;
	typedef int24_t ti_signed_int; /** @note Remember to account for integer promotion */
	typedef int32_t ti_signed_long;
	typedef int64_t ti_signed_long_long;

	typedef uint16_t ti_unsigned_short;
	typedef uint24_t ti_unsigned_int; /** @note Remember to account for integer promotion */
	typedef uint32_t ti_unsigned_long;
	typedef uint64_t ti_unsigned_long_long;

#ifdef _EZ80
	typedef uint24_t packed_uint24_t;
	typedef int24_t packed_int24_t;
	typedef ti_int packed_ti_int;
	typedef ti_signed_int packed_ti_signed_int;
	typedef ti_unsigned_int packed_ti_unsigned_int;

	typedef uint48_t packed_uint48_t;
	typedef int48_t packed_int48_t;

#else
	typedef struct packed_uint24_t {
		uint8_t do_not_access_these_bytes_directly[3];
	} packed_uint24_t; /** Use for indexing arrays */
	typedef packed_uint24_t packed_int24_t;
	typedef packed_uint24_t packed_ti_int;
	typedef packed_uint24_t packed_ti_signed_int;
	typedef packed_uint24_t packed_ti_unsigned_int;

	typedef struct packed_uint48_t {
		uint8_t do_not_access_these_bytes_directly[6];
	} packed_uint48_t; /** Use for indexing arrays */
	typedef packed_uint48_t packed_int48_t;

#endif

#ifdef _EZ80

	/**
	 * @brief Access pointers through this macro
	 */
	#define RAM_ADDRESS(x) ((void*)(x))
	
	/**
	 * @brief Calculate pointer offsets from this macro
	 */
	#define RAM_OFFSET(ptr) ((uint24_t)(ptr))

	#define PortCE_initialize(...)

	#define PortCE_terminate()

	#define PortCE_new_frame()

	#define PortCE_update_registers() 0

#else

	/**
	 * @brief 
	 * 
	 * @param address 
	 * @return void* 
	 */
	void* RAM_ADDRESS(uint24_t address);
	
	/**
	 * @brief 
	 * 
	 * @param ptr 
	 * @return uint24_t 
	 */
	uint24_t RAM_OFFSET(const void* restrict ptr);

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
	void PortCE_pace_frame(void);

	/**
	 * @brief Used to update registers
	 * 
	 * @return This function always returns  0
	 */
	ti_int PortCE_update_registers(void);

#endif

/* Clock */

	#define TI_CLOCKS_PER_SEC 32768
	#ifdef _EZ80
		typedef clock_t ti_clock_t;
		#define ti_clock() clock()
	#else
		typedef ti_long ti_clock_t;
		ti_clock_t ti_clock(void);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_H */
