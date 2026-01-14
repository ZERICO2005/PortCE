#include <stdlib.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <compression.h>
#include <PortCE.h>
#include <stdio.h>
#include <string.h>

#include "gfx/gfx.h"

#include "truth.h"

#include <time.h>

static inline int64_t getNanoTime(void) {
    struct timespec tp;
    if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
        int64_t nanoTime = (int64_t)tp.tv_sec * (int64_t)1000000000 + (int64_t)tp.tv_nsec;
        return nanoTime;
    } else {
        perror("clock_gettime");
        return 0;
    }
}

int main(void) {
    PortCE_initialize("zx7_test");
    gfx_Begin();

    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    PortCE_new_frame();

    const int test_count = 100;
    int64_t t0 = getNanoTime();
    for (int i = 0; i < test_count; i++) {
        zx7_Decompress(gfx_vram, background_compressed);
    }
    int64_t t1 = getNanoTime();
    double average_time_ms = ((double)(t1 - t0) * 1.0e-6) / (double)test_count;
    printf("average decompression time: %.3lfms\n", average_time_ms);

    PortCE_new_frame();

    int cmp = memcmp(gfx_vram, truth, GFX_LCD_WIDTH * GFX_LCD_HEIGHT);

    if (cmp == 0) {
        printf("Passed test\n");
    } else {
        printf("Failed: memcmp returned %d\n", cmp);
    }

    while (!os_GetCSC());

    gfx_End();

    PortCE_terminate();

    return 0;
}
