/**
 * @file
 *
 * @author "zerico2005"
 */

#ifndef PORTCE_H
#define PORTCE_H

#include <stdint.h>

/**
 * @file
 *
 * @author "zerico2005"
 */

#ifndef PORTCE_TYPE_H
#define PORTCE_TYPE_H

#include <stdint.h>

//------------------------------------------------------------------------------
// integral types
//------------------------------------------------------------------------------

#ifndef _EZ80

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

#else /* _EZ80 */

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

#endif /* _EZ80 */

/* eZ80 Integer types */

typedef int16_t ez80_short;
typedef int24_t ez80_int; /** @note Remember to account for integer promotion */
typedef int32_t ez80_long;
typedef int64_t ez80_long_long;

typedef uint16_t ez80_unsigned_short;
typedef uint24_t ez80_unsigned_int; /** @note Remember to account for integer promotion */
typedef uint32_t ez80_unsigned_long;
typedef uint64_t ez80_unsigned_long_long;

typedef ez80_short ez80_signed_short;
typedef ez80_int ez80_signed_int; /** @note Remember to account for integer promotion */
typedef ez80_long ez80_signed_long;
typedef ez80_long_long ez80_signed_long_long;

/* TI Integer types */

typedef ez80_short ti_short;
typedef int24_t ti_int; /** @note Remember to account for integer promotion */
typedef ez80_long ti_long;
typedef ez80_long_long ti_long_long __attribute__((__deprecated__("use ez80_long_long instead")));

typedef ez80_unsigned_short ti_unsigned_short;
typedef uint24_t ti_unsigned_int; /** @note Remember to account for integer promotion */
typedef ez80_unsigned_long ti_unsigned_long;
typedef ez80_unsigned_long_long ti_unsigned_long_long __attribute__((__deprecated__("use ez80_unsigned_long_long instead")));

typedef ti_short ti_signed_short;
typedef ti_int ti_signed_int; /** @note Remember to account for integer promotion */
typedef ti_long ti_signed_long;
typedef ti_long_long ti_signed_long_long __attribute__((__deprecated__("use ez80_signed_long_long instead")));

#ifdef _EZ80

typedef uint24_t packed_uint24_t;
typedef int24_t packed_int24_t;

typedef ti_int packed_ti_int;
typedef ti_signed_int packed_ti_signed_int;
typedef ti_unsigned_int packed_ti_unsigned_int;

typedef uint48_t packed_uint48_t;
typedef int48_t packed_int48_t;

#else /* _EZ80 */

typedef struct __attribute__((__packed__)) packed_uint24_t {
    uint8_t do_not_access_these_bytes_directly[3];
} packed_uint24_t;

typedef struct __attribute__((__packed__)) packed_int24_t {
    uint8_t do_not_access_these_bytes_directly[3];
} packed_int24_t;

typedef packed_uint24_t packed_ti_int;
typedef packed_uint24_t packed_ti_signed_int;
typedef packed_uint24_t packed_ti_unsigned_int;

typedef struct __attribute__((__packed__)) packed_uint48_t {
    uint8_t do_not_access_these_bytes_directly[6];
} packed_uint48_t;

typedef struct __attribute__((__packed__)) packed_int48_t {
    uint8_t do_not_access_these_bytes_directly[6];
} packed_int48_t;

#endif /* _EZ80 */

//------------------------------------------------------------------------------
// floating point types
//------------------------------------------------------------------------------

#ifdef _EZ80

/** ieee binary32 */
typedef float ti_float;
/** ieee binary32 (Alias for float) */
typedef double ti_double;
/** ieee binary64 */
typedef long double ti_long_double;

#else /* _EZ80 */

/** ieee binary32 */
typedef float ti_float;
/** ieee binary32 (Alias for float) */
typedef float ti_double;
/** ieee binary64 */
typedef double ti_long_double;

#endif /* _EZ80 */

//------------------------------------------------------------------------------
// absolute value
//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

inline int48_t i48abs(int48_t value) {
    return (value < 0) ? -value : value;
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

inline int24_t abs(int24_t value) {
    return (value < 0) ? -value : value;
}

inline int48_t abs(int48_t value) {
    return i48abs(value);
}

#endif /* __cplusplus */

#endif /* PORTCE_TYPE_H */

//------------------------------------------------------------------------------
// pointer macros
//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _EZ80

/**
 * @brief Access pointers through this macro
 */
#define RAM_ADDRESS(x) ((void*)(x))

/**
 * @brief Access pointers through this macro at compile time
 */
#define CONST_ADDRESS(x) ((void*)(x))

/**
 * @brief Calculate pointer offsets from this macro
 */
#define RAM_OFFSET(ptr) ((uintptr_t)(ptr))

#define PortCE_set_window_title(...)

#define PortCE_initialize(...)

#define PortCE_terminate()

#define PortCE_pace_frame(...)

#define PortCE_new_frame()

#define PortCE_update_registers() (0)

#else /* _EZ80 */

extern uint8_t simulated_ram[16777216];

void* RAM_ADDRESS(uint24_t address);

#define CONST_ADDRESS(x) ((void*)simulated_ram[(x)])

uint24_t RAM_OFFSET(const void* ptr);

/**
 * @brief Access a constant address through this macro
 */
#define RAM_ADDRESS_CONST(x) ((void*)(&simulated_ram[x]))

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

#endif /* _EZ80 */

/* Clock */

#ifndef _EZ80
    #define TI_CLOCKS_PER_SEC 32768
    typedef ti_long ti_clock_t;
    ti_clock_t ti_clock(void);
#else /* _EZ80 */
    #define TI_CLOCKS_PER_SEC CLOCKS_PER_SEC
    #define ti_clock_t clock_t
    #define ti_clock() clock()
#endif /* _EZ80 */

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------

#endif /* PORTCE_H */
