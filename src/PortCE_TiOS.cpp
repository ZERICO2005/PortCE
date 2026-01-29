/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"
#include <ti/ui.h>
#include <ti/vars.h>
#include <ti/screen.h>
#include <sys/lcd.h>

#include "PortCE_TiOS_Font.h"

#include "os_flags.hpp"

#include "PortCE_memory.hpp"
#include "ti84pceg.hpp"

#include <cstring>
#include <cstdio>
#include <cctype>

/**
 * @todo Implement TiOS functions
 *
 */

/* TiOS Colors */

#define STATUS_BAR_HEIGHT 30
#define CRSR_COL_OFFSET 2
#define CRSR_ROW_OFFSET 7
#define CRSR_WIDTH 10
#define CRSR_HEIGHT 14
#define CRSR_COL_WIDTH 11
#define CRSR_ROW_HEIGHT 20
#define MAX_ROWS 10
#define MAX_COLS 25

enum TiOS_Colors {
    TiOS_BLUE      = 0x001F,
    TiOS_RED       = 0x7C00,
    TiOS_BLACK     = 0x0000,
    TiOS_MAGENTA   = 0x7C1F,
    TiOS_GREEN     = 0x8260,
    TiOS_ORANGE    = 0xFE24,
    TiOS_BROWN     = 0x5880,
    TiOS_NAVY      = 0x0010,
    TiOS_LTBLUE    = 0x025F,
    TiOS_YELLOW    = 0xFFE0,
    TiOS_WHITE     = 0xFFFF,
    TiOS_LTGRAY    = 0x6318,
    TiOS_MEDGRAY   = 0x739C,
    TiOS_GRAY      = 0x4631,
    TiOS_DARKGRAY  = 0xA94A,
    TiOS_HIGHLIGHT = 0xBFFE
};

/* Temporary Graphics */
    static uint16_t gColor = TiOS_BLACK;

    static void fillRect(uint24_t x1, uint24_t y1, uint24_t x2, uint24_t y2) { //x start, y start, x length, y length
        uint16_t* fill = (uint16_t*)lcd_Ram + (int24_t)(x1 + (y1 * LCD_RESX));
        for (uint24_t dY = 0; dY < y2; dY++) {
            memset(fill,gColor,x2 * sizeof(uint16_t));
            fill += LCD_RESX;
        }
    }
    __attribute__((unused)) static void horiz(uint24_t x1, uint24_t y, uint24_t x2) { //x start, y postion, x length
        uint16_t* fill = (uint16_t*)lcd_Ram + (int24_t)(x1 + (y * LCD_RESX));
        memset(fill,gColor,x2 * sizeof(uint16_t));
    }
    __attribute__((unused)) static void vert(uint24_t x, uint24_t y1, uint24_t y2) { //x postion, y start, y length
        uint16_t* z = (uint16_t*)lcd_Ram + (int24_t)(x + ((y2 + y1) * LCD_RESX));
        for (uint16_t* fill = (uint16_t*)lcd_Ram + (y1 * LCD_RESX) + x; fill < z; fill += LCD_RESX) {
            *fill = gColor;
        }
    }
    __attribute__((unused)) static void fillScreen() {
        for (size_t i = 0; i < LCD_RESX * LCD_RESY; i++) {
            reinterpret_cast<uint16_t*>(lcd_Ram)[i] = gColor;
        }
    }

    static bool out_of_screen_bounds(void* ptr) {
        if (ptr < RAM_ADDRESS(ti::vRam)) {
            return true;
        }
        if (ptr >= RAM_ADDRESS(ti::vRamEnd)) {
            return true;
        }
        return false;
    }

    static void TiOS_large_font(uint24_t x, uint24_t y, uint8_t lexicon) { //x position, y position, letter index
        __attribute__((unused)) constexpr uint8_t data_width  = 12;
        constexpr uint8_t data_height = 16;
        constexpr uint8_t font_width  = 10;
        constexpr uint8_t font_height = 14;
        uint16_t const * src = TiOS_font_16x12 + (lexicon * data_height);
        uint16_t * dst = reinterpret_cast<uint16_t*>(lcd_Ram) + (y * LCD_RESX + x);
        for (uint8_t h = 0; h < font_height; h++) {
            uint16_t data = *src;
            for (uint8_t w = 0; w < font_width; w++) {
                if (out_of_screen_bounds(dst)) {
                    return;
                }
                *dst = (data & 1) ? gColor : TiOS_WHITE;
                data >>= 1;
                dst++;
            }
            src++;
            dst += (LCD_RESX - font_width);
        }
    }

    static int TiOS_putchar(int c) {
        gColor = TiOS_BLACK;
        if (isprint(c)) {
            if (os_CurCol >= MAX_COLS) {
                os_NewLine();
            }
            uint24_t posX = os_CurCol * CRSR_COL_WIDTH + CRSR_COL_OFFSET;
            uint24_t posY = os_CurRow * CRSR_ROW_HEIGHT + CRSR_ROW_OFFSET + STATUS_BAR_HEIGHT;
            TiOS_large_font(posX, posY, (uint8_t)c);
            os_CurCol++;
            return 0;
        }
        switch (c) {
            case '\n': os_NewLine(); break;
        }
        return 0;
    }

    __attribute__((unused))
    static int TiOS_puts(const char *str) {
        while (*str != '\0') {
            TiOS_putchar(*str);
            str++;
        }
        TiOS_putchar('\n');
        return 0;
    }

    __attribute__((unused))
    static int TiOS_vprintf(
        const char *__restrict format, va_list va
    ) {
        char buf[1024];
        memset(buf, '\0', sizeof(buf));
        vsnprintf(buf, sizeof(buf), format, va);
        size_t i = 0;
        while (buf[i] != '\0' && i < sizeof(buf)) {
            TiOS_putchar(buf[i]);
            i++;
        }
        return 0;
    }

/* <ti/ui.h> */

void boot_ClearVRAM(void) {
    PortCE_memory_memset(ti::vRam, 0xFF, LCD_SIZE);
}

void os_RunIndicOn(void) {
    gColor = TiOS_YELLOW;
    fillRect(LCD_RESX - 20,10,10,10);
}
void os_RunIndicOff(void) {
    gColor = TiOS_DARKGRAY;
    fillRect(LCD_RESX - 20,10,10,10);
}
void os_DrawStatusBar(void) {
    os_FillRectColor = TiOS_DARKGRAY;
    gColor = os_FillRectColor;
    fillRect(0, 0, LCD_RESX, STATUS_BAR_HEIGHT);
}
void os_ForceCmdNoChar(void) {
    // Not sure what this function does
}

/* <ti/screen.h> */

void boot_NewLine(void) {
    os_CurCol = 0;
    os_CurRow++;
    if (os_CurRow >= MAX_ROWS) {
        os_CurRow--;
        os_MoveUp();
    }
}

void os_NewLine(void) {
    os_CurCol = 0;
    os_CurRow++;
    if (os_CurRow >= MAX_ROWS) {
        os_CurRow--;
        os_MoveUp();
    }
}

void os_DisableCursor(void) {
    return;
}

void os_EnableCursor(void) {
    return;
}

void os_SetCursorPos(uint8_t curRow, uint8_t curCol) {
    os_CurRow = curRow;
    os_CurCol = curCol;
    if (os_CurRow >= MAX_ROWS) {
        os_CurRow = 0;
    }
    if (os_CurCol >= MAX_COLS) {
        os_CurCol = 0;
    }
}

void os_GetCursorPos(ti_uint *curRow, ti_uint *curCol) {
    if (curRow != nullptr) { *curRow = (ti_uint)os_CurRow; }
    if (curCol != nullptr) { *curCol = (ti_uint)os_CurCol; }
}

uint24_t os_PutStrFull(const char *str) {
    while (*str != '\0') {
        TiOS_putchar(*str);
        str++;
    }
    return 1;
}

uint24_t os_PutStrLine(const char *str) {
    while (*str != '\0') {
        TiOS_putchar(*str);
        str++;
    }
    os_NewLine();
    return 1;
}

void os_MoveUp(void) {
    uint16_t* const vram = (uint16_t*)lcd_Ram;
    uint16_t* dst;
    const uint16_t* src;
    dst = vram + (STATUS_BAR_HEIGHT * LCD_RESX);
    src = vram + ((STATUS_BAR_HEIGHT + CRSR_ROW_HEIGHT) * LCD_RESX);
    memmove(
        dst, src,
        LCD_RESX * (LCD_RESY - STATUS_BAR_HEIGHT - CRSR_ROW_HEIGHT) * sizeof(uint16_t)
    );
    memset(
        vram + LCD_RESX * (LCD_RESY - CRSR_ROW_HEIGHT), 0xFF,
        LCD_RESX * (STATUS_BAR_HEIGHT + CRSR_ROW_HEIGHT) * sizeof(uint16_t)
    );
}

void os_MoveDown(void) {
    uint16_t* const vram = (uint16_t*)lcd_Ram;
    uint16_t* dst;
    const uint16_t* src;
    dst = vram + ((STATUS_BAR_HEIGHT + CRSR_ROW_HEIGHT) * LCD_RESX);
    src = vram + (STATUS_BAR_HEIGHT * LCD_RESX);
    memmove(
        dst, src,
        LCD_RESX * (LCD_RESY - STATUS_BAR_HEIGHT - CRSR_ROW_HEIGHT) * sizeof(uint16_t)
    );
    memset(
        vram + LCD_RESX * (STATUS_BAR_HEIGHT), 0xFF,
        LCD_RESX * (STATUS_BAR_HEIGHT + CRSR_ROW_HEIGHT) * sizeof(uint16_t)
    );
}

void os_HomeUp(void) {
    os_CurRow = 0;
    os_CurCol = 0;
}

void os_ClrLCDFull(void) {
    gColor = TiOS_WHITE;
    fillRect(0, STATUS_BAR_HEIGHT, LCD_RESX, LCD_RESY - STATUS_BAR_HEIGHT);
    PortCE_new_frame();
}

void os_ClrLCD(void) {
    gColor = TiOS_WHITE;
    fillRect(0, STATUS_BAR_HEIGHT, LCD_RESX, LCD_RESY - STATUS_BAR_HEIGHT);
    PortCE_new_frame();
}

void os_ClrTxtShd(void) {
    PortCE_memory_memset(ti::cmdPixelShadow, 0xFF, 8400);
    PortCE_memory_memset(ti::pixelShadow   , 0xFF, 8400);
    PortCE_memory_memset(ti::pixelShadow2  , 0xFF, 8400);
}

void os_DisableHomeTextBuffer(void) {
    iy_flag_res(1, +0x0D); // no text buffer
    iy_flag_res(3, +0x4A); // use first shadow buffer
    iy_flag_set(5, +0x4C); // only display
}

void os_EnableHomeTextBuffer(void) {
    iy_flag_set(1, +0x0D); // use text buffer
    iy_flag_res(3, +0x4A); // use first shadow buffer
    iy_flag_res(5, +0x4C); // use shadow buffer
}

void os_GetStringInput(const char *prompt, char *buf, size_t bufsize) {
    os_PutStrFull(prompt);
    if (buf != nullptr && bufsize > 0) {
        *buf = '\0';
    }
    return;
}

size_t os_GetTokenInput(const char *prompt, __attribute__((unused)) void *buf, __attribute__((unused)) size_t bufsize) {
    os_PutStrFull(prompt);
    return 0;
}


void os_FontSelect(__attribute__((unused)) font_t *font) {
    return;
}

font_t *os_FontGetID(void) {
    return nullptr;
}

uint24_t os_FontGetWidth(const char *string) {
    return static_cast<uint24_t>(strlen(string) * 10);
}

uint24_t os_FontGetHeight(void) {
    return 16;
}

uint24_t os_FontDrawText(const char *str, uint16_t col, uint8_t row) {
    os_PenCol = col;
    os_PenRow = row;
    os_SetCursorPos(static_cast<uint8_t>(row / CRSR_HEIGHT), static_cast<uint8_t>(col / CRSR_WIDTH));
    while (*str != '\0') {
        TiOS_putchar(*str);
    }
    return row;
}

uint24_t os_FontDrawTransText(const char *str, uint16_t col, uint8_t row) {
    os_PenCol = col;
    os_PenRow = row;
    os_SetCursorPos(static_cast<uint8_t>(row / CRSR_HEIGHT), static_cast<uint8_t>(col / CRSR_WIDTH));
    while (*str != '\0') {
        TiOS_putchar(*str);
    }
    return row;
}

void os_SetDrawFGColor(uint24_t color) {
    os_DrawFGColor = (uint16_t)color;
}

uint24_t os_GetDrawFGColor(void) {
    return os_DrawFGColor;
}

void os_SetDrawBGColor(uint24_t color) {
    os_DrawBGColor = (uint16_t)color;
}

uint24_t os_GetDrawBGColor(void) {
    return os_DrawBGColor;
}
