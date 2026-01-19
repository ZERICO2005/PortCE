/**
 * @file
 *
 * @author zerico2005
 */

#ifndef PORTCE_COMMON_H
#define PORTCE_COMMON_H

#ifdef PORTCE_H
#error "#include <PortCE.h> must be done last"
#endif

#ifndef PORTCE_INTERNAL
#define PORTCE_INTERNAL
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdio.h>
#include <time.h>

#include <PortCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Debug */

// #define Debug_Print_LCD_Registers

/* Typedefs */

    /** Used for timestamps */
    typedef int64_t nano64_t;

    /** Intended to be the same type as enum */
    typedef int int_enum;

/* Functions */

    #define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
    #define FREE(x) do { free(x); x = nullptr; } while(0)

    #define MACRO_TO_STR(x) #x
    #define MACRO_TO_NUM(x) MACRO_TO_STR(x)

    // Left Circular Shift
    #define ROL(n,b) (((n) << (b)) | ((n) >> ((sizeof(n) * CHAR_BIT) - (b))))
    // Right Circular Shift
    #define ROR(n,b) (((n) >> (b)) | ((n) << ((sizeof(n) * CHAR_BIT) - (b))))

    #define linearInterpolation(x,x0,x1,y0,y1) ( (y0) + ( (((y1) - (y0)) * ((x) - (x0))) / ((x1) - (x0)) ) )
    #define linearInterpolationClamp(x,x0,x1,y0,y1) ( ((x) <= (x0)) ? (y0) : ( ((x) >= (x1)) ? (y1) : linearInterpolation((x),(x0),(x1),(y0),(y1)) ) )

/* Time */

    static inline nano64_t getNanoTime(void) {
        struct timespec tp;
        if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
            nano64_t nanoTime = (nano64_t)tp.tv_sec * (nano64_t)1000000000 + (nano64_t)tp.tv_nsec;
            return nanoTime;
        } else {
            perror("clock_gettime");
            return 0;
        }
    }

    #define NANO_TO_SECONDS(t) ((double)(t) / 1.0e9)
    #define NANO_TO_FRAMERATE(t) (1.0e9 / (double)(t))
    #define SECONDS_TO_NANO(s) ((nano64_t)((s) * 1.0e9))
    #define SECONDS_TO_FRAMERATE(s) (1.0 / (s))
    #define FRAMERATE_TO_NANO(f) ((nano64_t)(1.0e9 / (f)))
    #define FRAMERATE_TO_SECONDS(f) (1.0 / (f))

    static inline double getDecimalTime(void) {
        return NANO_TO_SECONDS(getNanoTime());
    }

/* PortCE */

    #define VIDEO_CHANNELS 4

    #define LCD_RESX (320)
    #define LCD_RESY (240)

    typedef struct PortCE_Config {
        uint8_t window_scale;      /**< Set the initial window scaling (1-8) */
        bool full_screen;          /**< (Unimplemented) Opens the window in full-screen */
        bool integer_scaling;      /**< (Unimplemented) Forces integer scaling */
        bool stretch_frames;       /**< (Unimplemented) Stretches the frames to the window size instead of displaying black borders */
        bool linear_interpolation; /**< Use linear interpolation instead of nearest neighbor */
    } PortCE_Config;

    extern double Ti84CE_Clockspeed;

    extern uint8_t simulated_ram[16777216];

/* Function Declarations */

void PortCE_terminate_fileioc(void);

void reset_ti84ce_registers(void);

void set_clockspeed(double clock_speed);

double get_clockspeed();

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_COMMON_H */
