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

#include <stdint.h>
#include <stdlib.h>

#include <PortCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Debug */

// #define Debug_Print_LCD_Registers

/* Functions */

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
#define FREE(x) do { free(x); x = nullptr; } while(0)

#define MACRO_TO_STR(x) #x
#define MACRO_TO_NUM(x) MACRO_TO_STR(x)

/* PortCE */

constexpr int LCD_RESX = (320);
constexpr int LCD_RESY = (240);

typedef struct PortCE_Config {
    uint8_t window_scale;      /**< Set the initial window scaling (1-8) */
    bool full_screen;          /**< (Unimplemented) Opens the window in full-screen */
    bool integer_scaling;      /**< (Unimplemented) Forces integer scaling */
    bool stretch_frames;       /**< (Unimplemented) Stretches the frames to the window size instead of displaying black borders */
    bool linear_interpolation; /**< Use linear interpolation instead of nearest neighbor */
} PortCE_Config;

/* Function Declarations */

void PortCE_terminate_fileioc(void);

void reset_ti84ce_registers(void);

void set_clockspeed(double clock_speed);

double get_clockspeed();

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_COMMON_H */
