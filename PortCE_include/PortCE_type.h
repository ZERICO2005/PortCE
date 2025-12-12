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

#endif

/* eZ80 Integer types */

    typedef int16_t ez80_short;
    typedef int24_t ez80_int; /** @note Remember to account for integer promotion */
    typedef int32_t ez80_long;
    typedef int64_t ez80_long_long;

    typedef int16_t ez80_signed_short;
    typedef int24_t ez80_signed_int; /** @note Remember to account for integer promotion */
    typedef int32_t ez80_signed_long;
    typedef int64_t ez80_signed_long_long;

    typedef uint16_t ez80_unsigned_short;
    typedef uint24_t ez80_unsigned_int; /** @note Remember to account for integer promotion */
    typedef uint32_t ez80_unsigned_long;
    typedef uint64_t ez80_unsigned_long_long;

/* TI Integer types */

    typedef int16_t ti_short;
    typedef int32_t ti_int; /** @note Remember to account for integer promotion */
    typedef int32_t ti_long;
    typedef int64_t ti_long_long __attribute__((__deprecated__("use ez80_long_long instead")));

    typedef uint16_t ti_unsigned_short;
    typedef uint32_t ti_unsigned_int; /** @note Remember to account for integer promotion */
    typedef uint32_t ti_unsigned_long;
    typedef uint64_t ti_unsigned_long_long __attribute__((__deprecated__("use ez80_unsigned_long_long instead")));

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

#else

typedef struct __attribute__((__packed__)) packed_uint24_t {
    uint24_t value : 24;
} packed_uint24_t;

typedef struct __attribute__((__packed__)) packed_int24_t {
    int24_t value : 24;
} packed_int24_t;

typedef packed_uint24_t packed_ti_int;
typedef packed_uint24_t packed_ti_signed_int;
typedef packed_uint24_t packed_ti_unsigned_int;

typedef struct __attribute__((__packed__)) packed_uint48_t {
    uint48_t value : 48;
} packed_uint48_t;

typedef struct __attribute__((__packed__)) packed_int48_t {
    int48_t value : 48;
} packed_int48_t;

#endif

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
#else
    /** ieee binary32 */
    typedef float ti_float;
    /** ieee binary32 (Alias for float) */
    typedef float ti_double;
    /** ieee binary64 */
    typedef double ti_long_double;
#endif

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

//------------------------------------------------------------------------------
// (experimental) extended integer types
//------------------------------------------------------------------------------

#if defined(PORTCE_EXTENDED_INTEGERS)

    #ifndef _EZ80

        typedef signed _BitInt(72) int72_t;
        typedef unsigned _BitInt(72) uint72_t;

        typedef signed _BitInt(96) int96_t;
        typedef unsigned _BitInt(96) uint96_t;

        #define  INT72_MIN (( int72_t)( (unsigned __int128)1 << 71))
        #define  INT72_MAX (( int72_t)(((unsigned __int128)1 << 71) - 1))
        #define UINT72_MAX ((uint72_t)(((unsigned __int128)1 << 72) - 1))
        #define  INT72_C(C) (( int72_t)C)
        #define UINT72_C(C) ((uint72_t)C)

        #define  INT96_MIN (( int96_t)( (unsigned __int128)1 << 95))
        #define  INT96_MAX (( int96_t)(((unsigned __int128)1 << 95) - 1))
        #define UINT96_MAX ((uint96_t)(((unsigned __int128)1 << 96) - 1))
        #define  INT96_C(C) (( int96_t)C)
        #define UINT96_C(C) ((uint96_t)C)

        typedef   signed __int128  int_least72_t;
        typedef unsigned __int128 uint_least72_t;
        typedef   signed __int128   int_fast72_t;
        typedef unsigned __int128  uint_fast72_t;

        typedef   signed __int128  int_least96_t;
        typedef unsigned __int128 uint_least96_t;
        typedef   signed __int128   int_fast96_t;
        typedef unsigned __int128  uint_fast96_t;

        #define  INT_LEAST72_MIN  INT128_MIN
        #define  INT_LEAST72_MAX  INT128_MAX
        #define UINT_LEAST72_MAX UINT128_MAX
        #define   INT_FAST72_MIN  INT128_MIN
        #define   INT_FAST72_MAX  INT128_MAX
        #define  UINT_FAST72_MAX UINT128_MAX

        #define  INT_LEAST96_MIN  INT128_MIN
        #define  INT_LEAST96_MAX  INT128_MAX
        #define UINT_LEAST96_MAX UINT128_MAX
        #define   INT_FAST96_MIN  INT128_MIN
        #define   INT_FAST96_MAX  INT128_MAX
        #define  UINT_FAST96_MAX UINT128_MAX

    #else

        #ifdef          __INT72_TYPE__
        typedef        __INT_72_TYPE__                  int_fast72_t;
        #define      INT_FAST72_MIN                 (~__INT_FAST72_MAX__)
        #define      INT_FAST72_MAX                   __INT_FAST72_MAX__
        #define      INT_FAST72_WIDTH    __STDINT_WIDTH(INT_FAST72)
        typedef       __UINT_72_TYPE__                 uint_fast72_t;
        #define     UINT_FAST72_MAX                  __UINT_FAST72_MAX__
        #define     UINT_FAST72_WIDTH   __STDINT_WIDTH(UINT_FAST72)
        typedef        __INT_72_TYPE__                 int_least72_t;
        #define     INT_LEAST72_MIN                (~__INT_LEAST72_MAX__)
        #define     INT_LEAST72_MAX                  __INT_LEAST72_MAX__
        #define     INT_LEAST72_WIDTH   __STDINT_WIDTH(INT_LEAST72)
        typedef       __UINT_72_TYPE__                uint_least72_t;
        #define    UINT_LEAST72_MAX                 __UINT_LEAST72_MAX__
        #define    UINT_LEAST72_WIDTH  __STDINT_WIDTH(UINT_LEAST72)
        #endif        /*__INT72_TYPE_*/

        #ifdef   __SIZEOF_INT96__
        typedef        __INT_96_TYPE__                  int_fast96_t;
        #define      INT_FAST96_MIN                 (~__INT_FAST96_MAX__)
        #define      INT_FAST96_MAX                   __INT_FAST96_MAX__
        #define      INT_FAST96_WIDTH    __STDINT_WIDTH(INT_FAST96)
        typedef       __UINT_96_TYPE__                 uint_fast96_t;
        #define     UINT_FAST96_MAX                  __UINT_FAST96_MAX__
        #define     UINT_FAST96_WIDTH   __STDINT_WIDTH(UINT_FAST96)
        typedef        __INT_96_TYPE__                 int_least96_t;
        #define     INT_LEAST96_MIN                (~__INT_LEAST96_MAX__)
        #define     INT_LEAST96_MAX                  __INT_LEAST96_MAX__
        #define     INT_LEAST96_WIDTH   __STDINT_WIDTH(INT_LEAST96)
        typedef       __UINT_96_TYPE__                uint_least96_t;
        #define    UINT_LEAST96_MAX                 __UINT_LEAST96_MAX__
        #define    UINT_LEAST96_WIDTH  __STDINT_WIDTH(UINT_LEAST96)
        #endif /*__SIZEOF_INT96__*/

    #endif

    #ifdef _EZ80
        typedef uint72_t packed_uint72_t;
        typedef int72_t packed_int72_t;
        typedef uint96_t packed_uint96_t;
        typedef int96_t packed_int96_t;

    #else
        typedef struct packed_uint72_t {
            uint8_t do_not_access_these_bytes_directly[9];
        } packed_uint72_t; /** Use for indexing arrays */
        typedef packed_uint72_t packed_int72_t;

        typedef struct packed_uint48_t {
            uint8_t do_not_access_these_bytes_directly[12];
        } packed_uint96_t; /** Use for indexing arrays */
        typedef packed_uint96_t packed_int96_t;

    #endif

#endif

#endif /* PORTCE_TYPE_H */
