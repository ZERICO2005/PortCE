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

#ifdef _EZ80

//------------------------------------------------------------------------------
// Additional <stdint.h> types
//------------------------------------------------------------------------------

#ifdef          __INT24_TYPE__
typedef        __INT_24_TYPE__                  int_fast24_t;
#define      INT_FAST24_MIN                 (~__INT_FAST24_MAX__)
#define      INT_FAST24_MAX                   __INT_FAST24_MAX__
#define      INT_FAST24_WIDTH    __STDINT_WIDTH(INT_FAST24)
typedef       __UINT_24_TYPE__                 uint_fast24_t;
#define     UINT_FAST24_MAX                  __UINT_FAST24_MAX__
#define     UINT_FAST24_WIDTH   __STDINT_WIDTH(UINT_FAST24)
typedef        __INT_24_TYPE__                 int_least24_t;
#define     INT_LEAST24_MIN                (~__INT_LEAST24_MAX__)
#define     INT_LEAST24_MAX                  __INT_LEAST24_MAX__
#define     INT_LEAST24_WIDTH   __STDINT_WIDTH(INT_LEAST24)
typedef       __UINT_24_TYPE__                uint_least24_t;
#define    UINT_LEAST24_MAX                 __UINT_LEAST24_MAX__
#define    UINT_LEAST24_WIDTH  __STDINT_WIDTH(UINT_LEAST24)
#endif        /*__INT24_TYPE_*/

#ifdef   __SIZEOF_INT48__
typedef        __INT_48_TYPE__                  int_fast48_t;
#define      INT_FAST48_MIN                 (~__INT_FAST48_MAX__)
#define      INT_FAST48_MAX                   __INT_FAST48_MAX__
#define      INT_FAST48_WIDTH    __STDINT_WIDTH(INT_FAST48)
typedef       __UINT_48_TYPE__                 uint_fast48_t;
#define     UINT_FAST48_MAX                  __UINT_FAST48_MAX__
#define     UINT_FAST48_WIDTH   __STDINT_WIDTH(UINT_FAST48)
typedef        __INT_48_TYPE__                 int_least48_t;
#define     INT_LEAST48_MIN                (~__INT_LEAST48_MAX__)
#define     INT_LEAST48_MAX                  __INT_LEAST48_MAX__
#define     INT_LEAST48_WIDTH   __STDINT_WIDTH(INT_LEAST48)
typedef       __UINT_48_TYPE__                uint_least48_t;
#define    UINT_LEAST48_MAX                 __UINT_LEAST48_MAX__
#define    UINT_LEAST48_WIDTH  __STDINT_WIDTH(UINT_LEAST48)
#endif /*__SIZEOF_INT48__*/

//------------------------------------------------------------------------------
// eZ80 types
//------------------------------------------------------------------------------

typedef int8_t ez80_char;
typedef uint8_t ez80_uchar;
typedef int8_t ez80_schar;

typedef int16_t ez80_short;
typedef int24_t ez80_int; /** @note Remember to account for integer promotion */
typedef int32_t ez80_long;
typedef int64_t ez80_llong;

typedef uint16_t ez80_ushort;
typedef uint24_t ez80_uint; /** @note Remember to account for integer promotion */
typedef uint32_t ez80_ulong;
typedef uint64_t ez80_ullong;

//------------------------------------------------------------------------------
// TiOS types
//------------------------------------------------------------------------------

typedef ez80_short ti_short;
typedef ez80_int ti_int; /** @note Remember to account for integer promotion */
typedef ez80_long ti_long;

typedef ez80_ushort ti_ushort;
typedef ez80_uint ti_uint; /** @note Remember to account for integer promotion */
typedef ez80_ulong ti_ulong;

//------------------------------------------------------------------------------
// packed types
//------------------------------------------------------------------------------

typedef uint24_t packed_uint24_t;
typedef int24_t packed_int24_t;

#ifdef __SIZEOF_INT48__
typedef uint48_t packed_uint48_t;
typedef int48_t packed_int48_t;
#endif /* __SIZEOF_INT48__ */

//------------------------------------------------------------------------------
// floating point types
//------------------------------------------------------------------------------

/** ieee binary32 */
typedef float ti_float;
/** ieee binary32 (Alias for float) */
typedef double ti_double;
/** ieee binary64 */
typedef long double ti_long_double;

//------------------------------------------------------------------------------
// pointer macros
//------------------------------------------------------------------------------

/**
 * @brief Access pointers through this macro.
 * @example `*(uint8_t*)RAM_ADDRESS(0xD40000) = 0xFF`
 * @note returns NULL if the address is 0, invalid, or read-only.
 */
#define RAM_ADDRESS(x) ((void*)(x))

/**
 * @brief Access pointers through this macro.
 * @example `uint8_t value = *(const uint8_t*)CONST_RAM_ADDRESS(0xD40000)`
 * @note returns NULL if the address is 0 or invalid.
 */
#define CONST_RAM_ADDRESS(x) ((const void*)(x))

/**
 * @brief Access pointers through this macro at compile time.
 * @example `uint8_t * const vram = RAM_ADDRESS_COMPILETIME(0xD40000)`
 * @note This macro may not be supported in all PortCE addressing modes.
 */
#define RAM_ADDRESS_COMPILETIME(x) ((void*)(x))

/**
 * @brief Calculate pointer offsets from this macro.
 * @example `lcd_Upbase = RAM_OFFSET(lcd_Ram)`
 * @note returns 0 if the pointer is NULL or does not point to PortCE memory.
 */
#define RAM_OFFSET(ptr) ((uintptr_t)(ptr))

//------------------------------------------------------------------------------
// PortCE functions
//------------------------------------------------------------------------------

#define PortCE_set_window_title(...)

#define PortCE_initialize(...)

#define PortCE_terminate()

#define PortCE_pace_frame(...)

#define PortCE_new_frame()

#define PortCE_update_registers() (0)

#define PortCE_get_file_name(x) x

//------------------------------------------------------------------------------
// Function aliases
//------------------------------------------------------------------------------

#define TI_CLOCKS_PER_SEC CLOCKS_PER_SEC
#define ti_clock_t clock_t
#define ti_clock clock

#define ti_random random
#define ti_srandom srandom

//------------------------------------------------------------------------------

#else /* _EZ80 */

//------------------------------------------------------------------------------
// (u)int24_t and (u)int48_t
//------------------------------------------------------------------------------

typedef signed _BitInt(24) int24_t; /** @note Remember to account for integer promotion */
typedef unsigned _BitInt(24) uint24_t; /** @note Remember to account for integer promotion */

typedef signed _BitInt(48) int48_t;
typedef unsigned _BitInt(48) uint48_t;

#define  INT24_C(C) (( int24_t)INT32_C(C))
#define UINT24_C(C) ((uint24_t)UINT32_C(C))

#define  INT48_C(C) (( int48_t)INT64_C(C))
#define UINT48_C(C) ((uint48_t)UINT64_C(C))

#define  INT24_MIN  INT24_C(-8388608)
#define  INT24_MAX  INT24_C( 8388607)
#define UINT24_MAX UINT24_C(16777215)

#define  INT48_MIN  INT48_C(-140737488355328)
#define  INT48_MAX  INT48_C( 140737488355327)
#define UINT48_MAX UINT48_C( 281474976710655)

#define INT24_WIDTH (24)
#define UINT24_WIDTH (24)

#define INT48_WIDTH (48)
#define UINT48_WIDTH (48)

//------------------------------------------------------------------------------
// Additional <stdint.h> types
//------------------------------------------------------------------------------

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

#ifdef INT32_WIDTH
#define  INT_LEAST24_WIDTH  INT32_WIDTH
#define   INT_FAST24_WIDTH  INT32_WIDTH
#endif /* INT32_WIDTH */
#ifdef UINT32_WIDTH
#define UINT_LEAST24_WIDTH UINT32_WIDTH
#define  UINT_FAST24_WIDTH UINT32_WIDTH
#endif /* UINT32_WIDTH */

#ifdef INT64_WIDTH
#define  INT_LEAST48_WIDTH  INT64_WIDTH
#define   INT_FAST48_WIDTH  INT64_WIDTH
#endif /* INT64_WIDTH */
#ifdef UINT64_WIDTH
#define UINT_LEAST48_WIDTH UINT64_WIDTH
#define  UINT_FAST48_WIDTH UINT64_WIDTH
#endif /* UINT64_WIDTH */

//------------------------------------------------------------------------------
// eZ80 types
//------------------------------------------------------------------------------

typedef int8_t ez80_char;
typedef uint8_t ez80_uchar;
typedef int8_t ez80_schar;

typedef int16_t ez80_short;
typedef int24_t ez80_int; /** @note Remember to account for integer promotion */
typedef int32_t ez80_long;
typedef int64_t ez80_llong;

typedef uint16_t ez80_ushort;
typedef uint24_t ez80_uint; /** @note Remember to account for integer promotion */
typedef uint32_t ez80_ulong;
typedef uint64_t ez80_ullong;

//------------------------------------------------------------------------------
// TiOS types
//------------------------------------------------------------------------------

typedef ez80_short ti_short;
typedef int24_t ti_int; /** @note Remember to account for integer promotion */
typedef ez80_long ti_long;

typedef ez80_ushort ti_ushort;
typedef uint24_t ti_uint; /** @note Remember to account for integer promotion */
typedef ez80_ulong ti_ulong;

//------------------------------------------------------------------------------
// packed types
//------------------------------------------------------------------------------

typedef struct __attribute__((__packed__)) packed_uint24_t {
    uint8_t do_not_access_these_bytes_directly[3];
} packed_uint24_t;

typedef struct __attribute__((__packed__)) packed_int24_t {
    uint8_t do_not_access_these_bytes_directly[3];
} packed_int24_t;

typedef struct __attribute__((__packed__)) packed_uint48_t {
    uint8_t do_not_access_these_bytes_directly[6];
} packed_uint48_t;

typedef struct __attribute__((__packed__)) packed_int48_t {
    uint8_t do_not_access_these_bytes_directly[6];
} packed_int48_t;

//------------------------------------------------------------------------------
// floating point types
//------------------------------------------------------------------------------

/** ieee binary32 */
typedef float ti_float;
/** ieee binary32 (Alias for float) */
typedef float ti_double;
/** ieee binary64 */
typedef double ti_long_double;

//------------------------------------------------------------------------------
// absolute value
//------------------------------------------------------------------------------

inline int48_t i48abs(int48_t value) {
    return (value < 0) ? -value : value;
}

#ifdef __cplusplus

extern "C++" inline int24_t abs(int24_t value) {
    return (value < 0) ? -value : value;
}

extern "C++" inline int48_t abs(int48_t value) {
    return i48abs(value);
}

#endif /* __cplusplus */

//------------------------------------------------------------------------------
// pointer macros
//------------------------------------------------------------------------------

/**
 * @brief Access pointers through this macro.
 * @example `*(uint8_t*)RAM_ADDRESS(0xD40000) = 0xFF`
 * @note returns NULL if the address is 0, invalid, or read-only.
 */
void *RAM_ADDRESS(uint24_t address);

/**
 * @brief Access pointers through this macro.
 * @example `uint8_t value = *(const uint8_t*)CONST_RAM_ADDRESS(0xD40000)`
 * @note returns NULL if the address is 0 or invalid.
 */
const void *CONST_RAM_ADDRESS(uint24_t address);

/**
 * @warning Do NOT access this directly.
 */
extern uint8_t PortCE_static_linear_memory[16777216];

/**
 * @brief Access pointers through this macro at compile time.
 * @example `uint8_t * const vram = RAM_ADDRESS_COMPILETIME(0xD40000)`
 * @note This macro may not be supported in all PortCE addressing modes.
 */
#define RAM_ADDRESS_COMPILETIME(x) ((void*)(&PortCE_static_linear_memory[x]))

/**
 * @brief Calculate pointer offsets from this macro.
 * @example `lcd_Upbase = RAM_OFFSET(lcd_Ram)`
 * @note returns 0 if the pointer is NULL or does not point to PortCE memory.
 */
uint24_t RAM_OFFSET(const void *ptr);

//------------------------------------------------------------------------------
// PortCE functions
//------------------------------------------------------------------------------

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

/**
 * @brief Converts "MYAPPVAR" into a path like "./../src/gfx/MYAPPVAR.8xv",
 * returning NULL if the file cannot be found.
 * @example fopen(PortCE_get_appvar_path("MYAPPVAR"), "r")
 *
 * @returns file path string to appvar.
 * @warning The contents of the string/pointer are overwritten by subsequent
 * calls to the function.
 */
const char* PortCE_get_appvar_path(const char * name);

//------------------------------------------------------------------------------
// Function aliases
//------------------------------------------------------------------------------

#define TI_CLOCKS_PER_SEC 32768
typedef ti_long ti_clock_t;
ti_clock_t ti_clock(void);

//------------------------------------------------------------------------------

#endif /* _EZ80 */

//------------------------------------------------------------------------------
// Deprecated
//------------------------------------------------------------------------------

typedef ez80_ushort ez80_unsigned_short __attribute__((__deprecated__("use ez80_ushort instead")));
typedef ez80_uint ez80_unsigned_int __attribute__((__deprecated__("use ez80_uint instead"))); /** @note Remember to account for integer promotion */
typedef ez80_ulong ez80_unsigned_long __attribute__((__deprecated__("use ez80_ulong instead")));

typedef ti_ushort ti_unsigned_short __attribute__((__deprecated__("use ti_ushort instead")));
typedef ti_uint ti_unsigned_int __attribute__((__deprecated__("use ti_uint instead"))); /** @note Remember to account for integer promotion */
typedef ti_ulong ti_unsigned_long __attribute__((__deprecated__("use ti_ulong instead")));

typedef ti_short ti_signed_short __attribute__((__deprecated__("use ti_short instead")));
typedef ti_int ti_signed_int __attribute__((__deprecated__("use ti_int instead"))); /** @note Remember to account for integer promotion */
typedef ti_long ti_signed_long __attribute__((__deprecated__("use ti_long instead")));

typedef ez80_llong ez80_long_long __attribute__((__deprecated__("use ez80_llong instead")));
typedef ez80_ullong ez80_unsigned_long_long __attribute__((__deprecated__("use ez80_ullong instead")));
typedef ez80_llong ez80_signed_long_long __attribute__((__deprecated__("use ez80_llong instead")));

typedef ez80_llong ti_long_long __attribute__((__deprecated__("use ez80_llong instead")));
typedef ez80_ullong ti_unsigned_long_long __attribute__((__deprecated__("use ez80_ullong instead")));
typedef ez80_llong ti_signed_long_long __attribute__((__deprecated__("use ez80_llong instead")));

typedef packed_int24_t packed_ti_int __attribute__((__deprecated__("use packed_int24_t instead")));
typedef packed_int24_t packed_ti_signed_int __attribute__((__deprecated__("use packed_int24_t instead")));
typedef packed_uint24_t packed_ti_unsigned_int __attribute__((__deprecated__("use packed_uint24_t instead")));

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_H */
