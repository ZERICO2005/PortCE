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

#ifdef __cplusplus
extern "C" {
#endif

#define ti_malloc(x) malloc(x)
#define ti_free(x) free(x)

#ifndef _EZ80

	typedef signed _BitInt(24) int24_t; /** @note Remember to account for integer promotion */
	typedef unsigned _BitInt(24) uint24_t; /** @note Remember to account for integer promotion */
	
	typedef signed _BitInt(48) int48_t;
	typedef unsigned _BitInt(48) uint48_t;

	#define  INT24_MIN (-8388608)
	#define  INT24_MAX ( 8388607)
	#define UINT24_MAX (16777215)
	#define  INT24_C(C) (( int24_t)C)
	#define UINT24_C(C) ((uint24_t)C)

	#define  INT48_MIN (-140737488355328)
	#define  INT48_MAX ( 140737488355327)
	#define UINT48_MAX ( 281474976710655)
	#define  INT48_C(C) (( int48_t)C)
	#define UINT48_C(C) ((uint48_t)C)

	typedef  int32_t  int_least24_t;
	typedef uint32_t uint_least24_t;
	typedef  int32_t   int_fast24_t;
	typedef uint32_t  uint_fast24_t;

	typedef  int64_t  int_least48_t;
	typedef uint64_t uint_least48_t;
	typedef  int64_t   int_fast48_t;
	typedef uint64_t  uint_fast48_t;

	#define  INT_LEAST24_MIN  INT32_MIN
	#define  INT_LEAST24_MAX  INT32_MAX
	#define UINT_LEAST24_MAX UINT32_MAX
	#define   INT_FAST24_MIN  INT32_MIN
	#define   INT_FAST24_MAX  INT32_MAX
	#define  UINT_FAST24_MAX UINT32_MAX

	#define  INT_LEAST48_MIN  INT64_MIN
	#define  INT_LEAST48_MAX  INT64_MAX
	#define UINT_LEAST48_MAX UINT64_MAX
	#define   INT_FAST48_MIN  INT64_MIN
	#define   INT_FAST48_MAX  INT64_MAX
	#define  UINT_FAST48_MAX UINT64_MAX

#else

	#ifdef          __INT24_TYPE__
	typedef         __INT24_TYPE__                  int_fast24_t;
	#define      INT_FAST24_MIN                 (~__INT_FAST24_MAX__)
	#define      INT_FAST24_MAX                   __INT_FAST24_MAX__
	#define      INT_FAST24_WIDTH    __STDINT_WIDTH(INT_FAST24)
	typedef        __UINT24_TYPE__                 uint_fast24_t;
	#define     UINT_FAST24_MAX                  __UINT_FAST24_MAX__
	#define     UINT_FAST24_WIDTH   __STDINT_WIDTH(UINT_FAST24)
	typedef         __INT24_TYPE__                 int_least24_t;
	#define     INT_LEAST24_MIN                (~__INT_LEAST24_MAX__)
	#define     INT_LEAST24_MAX                  __INT_LEAST24_MAX__
	#define     INT_LEAST24_WIDTH   __STDINT_WIDTH(INT_LEAST24)
	typedef        __UINT24_TYPE__                uint_least24_t;
	#define    UINT_LEAST24_MAX                 __UINT_LEAST24_MAX__
	#define    UINT_LEAST24_WIDTH  __STDINT_WIDTH(UINT_LEAST24)
	#endif        /*__INT24_TYPE_*/
	
	#ifdef   __SIZEOF_INT48__
	typedef         __INT48_TYPE__                  int_fast48_t;
	#define      INT_FAST48_MIN                 (~__INT_FAST48_MAX__)
	#define      INT_FAST48_MAX                   __INT_FAST48_MAX__
	#define      INT_FAST48_WIDTH    __STDINT_WIDTH(INT_FAST48)
	typedef        __UINT48_TYPE__                 uint_fast48_t;
	#define     UINT_FAST48_MAX                  __UINT_FAST48_MAX__
	#define     UINT_FAST48_WIDTH   __STDINT_WIDTH(UINT_FAST48)
	typedef         __INT48_TYPE__                 int_least48_t;
	#define     INT_LEAST48_MIN                (~__INT_LEAST48_MAX__)
	#define     INT_LEAST48_MAX                  __INT_LEAST48_MAX__
	#define     INT_LEAST48_WIDTH   __STDINT_WIDTH(INT_LEAST48)
	typedef        __UINT48_TYPE__                uint_least48_t;
	#define    UINT_LEAST48_MAX                 __UINT_LEAST48_MAX__
	#define    UINT_LEAST48_WIDTH  __STDINT_WIDTH(UINT_LEAST48)
	#endif /*__SIZEOF_INT48__*/

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

/* Float types */

#ifdef _EZ80
	/** ieee binary32 */
	typedef float ti_float;
	/** ieee binary32 (Alias for float) */
	typedef double ti_double;
	/** ieee binary64 */
	typedef long double ti_long_double;
#else
	/** ieee binary32 */
	typedef float ti_float;
	/** ieee binary32 (Alias for float) */
	typedef float ti_double;
	/** ieee binary64 */
	typedef double ti_long_double;
#endif

#ifdef _EZ80

	/**
	 * @brief Access pointers through this macro
	 */
	#define RAM_ADDRESS(x) ((void*)(x))

	/**
	 * @brief Access a constant address through this macro
	 */
	#define RAM_ADDRESS_CONST(x) ((void*)(x))
	
	/**
	 * @brief Calculate pointer offsets from this macro
	 */
	#define RAM_OFFSET(ptr) ((uintptr_t)(ptr))

	#define PortCE_main main

	#define PortCE_set_window_title(...)

	#define PortCE_initialize(...)

	#define PortCE_terminate()

	#define PortCE_pace_frame(...)

	#define PortCE_new_frame()

	#define PortCE_update_registers() 0

#else

	/**
	 * @brief Do not access this array directly
	 */
	extern uint8_t simulated_ram[16777216];

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
	uint24_t RAM_OFFSET(const void* ptr);

	/**
	 * @brief Access a constant address through this macro
	 */
	#define RAM_ADDRESS_CONST(x) ((void*)(&simulated_ram[x]))

	#define PortCE_main main

	/**
	 * @brief Initializes the PortCE screen, keyboard, timers, etc.
	 * 
	 * @param window_title your programs name (Can be NULL)
	 */
	void PortCE_initialize(const char* window_title);

	/**
	 * @brief Set the window title for your program (window_title can be NULL)
	 */
	void PortCE_set_window_title(const char* window_title);

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

	#define TI_CLOCKS_PER_SEC 32768
	#ifdef _EZ80
		// typedef clock_t ti_clock_t;
		#define ti_clock_t clock_t
		#define ti_clock() clock()
	#else
		typedef ti_long ti_clock_t;
		ti_clock_t ti_clock(void);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_H */
