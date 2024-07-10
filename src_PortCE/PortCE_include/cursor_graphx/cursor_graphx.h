/**
 * @todo Come up with a better name for this library,
 * and improve the function naming.
 */
#ifndef CURSOR_GRAPHX_H
#define CURSOR_GRAPHX_H

/**
 * @brief cursor_graphx allows for easier use of the ti84ce's hardware cursor.
 * Use cursor_graphx for Row-Major, and cursor_graphy for Column-Major.
 */

#include <stdint.h>

#ifdef _EZ80
    #ifndef RAM_ADDRESS
        #define RAM_ADDRESS(x) ((void*)(x))
    #endif
    #ifndef RAM_OFFSET
        #define RAM_OFFSET(ptr) ((uint24_t)(ptr))
    #endif
#else
    #include <PortCE.h>
#endif

/* Library Setup */

    /**
     * @brief Initializes the hardware cursor and the `cursor_graphx` library
     * context.
     *
     * This function should be called before any other `cursor_graphx` library
     * routines.
     */
    void crsr_gfx_Begin(void);

    /**
     * @brief Ends the hardware cursor and `cursor_graphx` library context.
     *
     */
    void crsr_gfx_End(void);

/* Cursor Config */

    void crsr_gfx_Show(void);
    void crsr_gfx_Hide(void);
    void crsr_gfx_Toggle(void);

    #define Crsr_Size_32x32 (0)
    #define Crsr_Size_64x64 (1)
    #define Crsr_Size_Small (0)
    #define Crsr_Size_Large (1)

    void crsr_gfx_SetCursorSize(uint8_t crsr_size);

    void crsr_gfx_SetCursorIndex(uint8_t index);

    void crsr_gfx_SetPosition(int24_t x, int24_t y);

/* Colors/Palette */

    #define Crsr_Color_Primary      (0x0)
    #define Crsr_Color_Secondary    (0x1)
    #define Crsr_Color_Transparent  (0x2)
    #define Crsr_Color_Inverted     (0x3)

	void crsr_gfx_SetFillColor(uint8_t fill_color);

    void crsr_gfx_SetPrimaryColor(uint16_t color);

    void crsr_gfx_SetSecondaryColor(uint16_t color);

/* Basic Graphics */

    void crsr_gfx_SetPixel(uint8_t x, uint8_t y);

    uint8_t crsr_gfx_GetPixel(uint8_t x, uint8_t y);

    void crsr_gfx_FillScreen(void);

    void crsr_gfx_ZeroScreen(void);

    void crsr_gfx_Rectangle(
        int8_t x,
        int8_t y,
        int8_t width,
        int8_t height
    );

    void crsr_gfx_FillRectangle(
        int8_t x,
        int8_t y,
        int8_t width,
        int8_t height
    );

    void crsr_gfx_Rectangle(
        int8_t x,
        int8_t y,
        int8_t length
    );

    void crsr_gfx_HorizLine(
        int8_t x,
        int8_t y,
        int8_t length
    );
    
    void crsr_gfx_VertLine(
        int8_t x,
        int8_t y,
        int8_t length
    );

#endif /* CURSOR_GRAPHX_H */