#include "cursor_graphx.h"
#include <sys/lcd.h>
#include <string.h>

/* Internal */

    #define Crsr_Fill_Primary      (0x00)
    #define Crsr_Fill_Secondary    (0x55)
    #define Crsr_Fill_Transparent  (0xAA)
    #define Crsr_Fill_Inverted     (0xFF)

    static uint8_t crsr_gfx_fill_color = Crsr_Fill_Transparent;

/* Library Setup */

    void crsr_gfx_Begin(void) {
        crsr_gfx_fill_color = Crsr_Fill_Transparent;
        crsr_gfx_Hide();
        crsr_gfx_SetCursorSize(Crsr_Size_32x32);
        crsr_gfx_SetCursorIndex(0);
        crsr_gfx_ZeroScreen();
        lcd_CrsrClip = 0;
    }

    void crsr_gfx_End(void) {

    }

/* Cursor Config */

    void crsr_gfx_Show(void) {
        lcd_CrsrCtrl |= 0x1;
    }

    void crsr_gfx_Hide(void) {
        lcd_CrsrCtrl &= ~0x1;
    }

    void crsr_gfx_Toggle(void) {
        lcd_CrsrCtrl ^= 0x1;
    }

    void crsr_gfx_SetCursorSize(uint8_t crsr_size) {
        if (crsr_size == Crsr_Size_32x32) {
            lcd_CrsrConfig &= ~0x1;
        } else {
            lcd_CrsrConfig |= 0x1;
        }
    }

    __attribute__((unused)) static uint8_t crsr_gfx_GetCursorType() {
        return lcd_CrsrConfig & 0x1;
    }

    static uint8_t crsr_gfx_GetCursorSize() {
        return (lcd_CrsrConfig & 0x1) ? 32 : 64;
    }

    static uint24_t crsr_gfx_GetCursorLength() {
        return (lcd_CrsrConfig & 0x1) ? lcd_CrsrImageLen32 : lcd_CrsrImageLen64;
    }

    static uint8_t crsr_gfx_GetCursorPitch() {
        return (lcd_CrsrConfig & 0x1) ? (32 / 4) : (64 / 4);
    }

    static bool crsr_gfx_InsideClippingBound(uint8_t x, uint8_t y) {
        const uint8_t clip = (lcd_CrsrConfig & 0x1) ? 32 : 64;
        return (x < clip && y < clip) ? true : false;
    }

    static uint8_t* crsr_gfx_GetCursorOffset(void) {
        if (lcd_CrsrConfig & 0x1) {
            return lcd_CrsrImage;
        }
        return lcd_CrsrImage + lcd_CrsrImageLen32 * ((lcd_CrsrCtrl >> 4) & 0x3);
    }

    void crsr_gfx_SetCursorIndex(uint8_t index) {
        lcd_CrsrCtrl |= ((index) & 0x3) << 4;
    }

    void crsr_gfx_SetPosition(int24_t x, int24_t y) {
        lcd_CrsrX = (uint16_t)x;
        lcd_CrsrY = (uint8_t)y;
    }

/* Colors/Palette */

    void crsr_gfx_SetFillColor(uint8_t fill_color) {
        fill_color &= 0x3;
        fill_color |= (fill_color << 6) | (fill_color << 4) | (fill_color << 2);
    }

    void crsr_gfx_SetPrimaryColor(uint16_t color) {
        lcd_CrsrPalette0 = color;
    }

    void crsr_gfx_SetSecondaryColor(uint16_t color) {
        lcd_CrsrPalette1 = color;
    }

/* Basic Graphics */

    void crsr_gfx_SetPixel(uint8_t x, uint8_t y) {
        if (crsr_gfx_InsideClippingBound(x,y) == false) {
            return;
        }
        uint8_t* pixel = crsr_gfx_GetCursorOffset();
        pixel += (x >> 4) + y * crsr_gfx_GetCursorPitch();
        switch(x & 0x3) {
            case 0x0:
                *pixel = (*pixel & 0x3F) | (crsr_gfx_fill_color & 0xC0);
                return;
            case 0x1:
                *pixel = (*pixel & 0xCF) | (crsr_gfx_fill_color & 0x30);
                return;
            case 0x2:
                *pixel = (*pixel & 0xF3) | (crsr_gfx_fill_color & 0x0C);
                return;
            case 0x3:
                *pixel = (*pixel & 0xFC) | (crsr_gfx_fill_color & 0x03);
                return;
        }
    }

    uint8_t crsr_gfx_GetPixel(uint8_t x, uint8_t y) {
        if (crsr_gfx_InsideClippingBound(x,y) == false) {
            return 0xFF;
        }
        uint8_t* pixel = crsr_gfx_GetCursorOffset();
        pixel += (x >> 4) + y * crsr_gfx_GetCursorPitch();
        switch(x & 0x3) {
            case 0x0:
                return (*pixel >> 6) & 0x03;
            case 0x1:
                return (*pixel >> 4) & 0x03;
            case 0x2:
                return (*pixel >> 2) & 0x03;
            case 0x3:
                return *pixel & 0x03;
        }
        return 0xFF; // Unreachable
    }

    void crsr_gfx_FillScreen(void) {
        uint8_t* fill_ptr = crsr_gfx_GetCursorOffset();
        memset(fill_ptr, crsr_gfx_fill_color, crsr_gfx_GetCursorLength());
    }

    void crsr_gfx_ZeroScreen(void) {
        memset(lcd_CrsrImage, Crsr_Fill_Transparent, lcd_CrsrImageLen64);
    }

    void crsr_gfx_FillRectangle(
        int8_t x,
        int8_t y,
        int8_t width,
        int8_t height
    ) {
        if (width < 0) {
            x -= width;
            width = -width;
        }
        if (height < 0) {
            y -= height;
            height = -height;
        }

        const uint8_t clip = crsr_gfx_GetCursorSize();

        if (x >= clip || y >= clip) {
            return;
        }

        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }

        width = (x + width > clip) ? (clip - x) : width;
        height = (y + height > clip) ? (clip - y) : height;
        if (width <= 0 || height <= 0) {
            return;
        }

        for (uint8_t v = 0; v < height; v++) {
            for (uint8_t u = 0; u < width; u++) {
                crsr_gfx_SetPixel(x, y);
                x++;
            }
            x -= width;
            y++;
        }
    }

    void crsr_gfx_Rectangle(
        int8_t x,
        int8_t y,
        int8_t width,
        int8_t height
    ) {
        crsr_gfx_HorizLine(x            , y             , width );
        crsr_gfx_HorizLine(x            , y + height - 1, width );
        crsr_gfx_VertLine (x            , y             , height);
        crsr_gfx_VertLine (x + width - 1, y             , height);
    }

    void crsr_gfx_HorizLine(
        int8_t x,
        int8_t y,
        int8_t length
    ) {
        if (length < 0) {
            x -= length;
            length = -length;
        }
        const uint8_t clip = crsr_gfx_GetCursorSize();
        if (x >= clip || y >= clip) {
            return;
        }
        if (x < 0) {
            length += x;
            x = 0;
        }
        length = (x + length > clip) ? (clip - x) : length;
        if (length <= 0) {
            return;
        }
        for (uint8_t u = 0; u < length; u++) {
            crsr_gfx_SetPixel(x, y);
            x++;
        }
    }

    void crsr_gfx_VertLine(
        int8_t x,
        int8_t y,
        int8_t length
    ) {
        if (length < 0) {
            y -= length;
            length = -length;
        }
        const uint8_t clip = crsr_gfx_GetCursorSize();
        if (y >= clip || x >= clip) {
            return;
        }
        if (y < 0) {
            length += y;
            y = 0;
        }
        length = (y + length > clip) ? (clip - y) : length;
        if (length <= 0) {
            return;
        }
        for (uint8_t v = 0; v < length; v++) {
            crsr_gfx_SetPixel(x, y);
            y++;
        }
    }
