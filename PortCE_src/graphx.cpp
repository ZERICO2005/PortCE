#include "lib_graphx.hpp"

template<>
void GraphX::gfz_SetPixel_NoClip(uint32_t x, uint8_t y, uint8_t color) const {
    if (x < GFX_LCD_WIDTH && y < GFX_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint24_t)x + (y * GFX_LCD_WIDTH)] = color;
    }
}

#if 0
#include <PortCE.h>

#include <graphx.h>

#include "graphz.hpp"
#include "graphx_data.h"

#define lcd_Control              (*(volatile uint24_t*)RAM_ADDRESS(0xE30018))
#define lcd_VideoMode            (*(volatile uint16_t*)RAM_ADDRESS(0xE30018))
#define lcd_UpBase               (*(volatile uint24_t*)RAM_ADDRESS(0xE30010))
#define lcd_LpBase               (*(volatile uint24_t*)RAM_ADDRESS(0xE30014))
#define lcd_BGR8bit 0x927
#define lcd_BGR16bit 0x92D

struct GraphX_Type {
    // using region = gfx_region_t;
    // using sprite = gfx_sprite_t;
    // using rletsprite = gfx_rletsprite_t;
    // using tilemap = gfx_tilemap_t;cd
};

typedef GraphZ<GraphX_Type> GraphX;

template<>
void GraphX::gfz_SetPixel_NoClip(ti_unsigned_int x, uint8_t y, uint8_t color) {
    if (x < GFX_LCD_WIDTH && y < GFX_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint24_t)x + (y * GFX_LCD_WIDTH)] = color;
    }
}

static GraphX lib("graphx", gfx_DefaultCharSpacing, gfx_DefaultTextData);

//------------------------------------------------------------------------------
// Colors and Palette
//------------------------------------------------------------------------------

uint8_t gfx_SetColor(uint8_t index) {
    return lib.gfz_SetColor(index);
}

uint8_t gfx_SetTransparentColor(uint8_t index) {
    return lib.gfz_SetTransparentColor(index);
}

uint8_t gfx_SetTextFGColor(uint8_t color) {
    return lib.gfz_SetTextFGColor(color);
}

uint8_t gfx_SetTextBGColor(uint8_t color) {
    return lib.gfz_SetTextBGColor(color);
}

uint8_t gfx_SetTextTransparentColor(uint8_t color) {
    return lib.gfz_SetTextTransparentColor(color);
}

void gfx_SetPalette(const void *palette, size_t palette_size, uint8_t offset) {
    lib.gfz_SetPalette(palette, palette_size, offset);
}

void gfx_SetDefaultPalette(gfx_mode_t mode) {
    lib.gfz_SetDefaultPalette(static_cast<gfz_mode_t>(mode));
}

uint16_t gfx_Darken(uint16_t color, uint8_t amount) {
    return lib.gfz_Darken(color, amount);
}

uint16_t gfx_Lighten(uint16_t color, uint8_t amount) {
    return lib.gfz_Lighten(color, amount);
}

//------------------------------------------------------------------------------
// Rendering Primitives
//------------------------------------------------------------------------------

void gfx_Begin() {
    lib.gfz_Begin();
    lcd_VideoMode = lcd_BGR8bit;
    lib.CharSpacing = gfx_DefaultCharSpacing;
    lib.TextData = gfx_DefaultTextData;
}

void gfx_End(void) {
    lcd_VideoMode = lcd_BGR16bit;
    lib.gfz_End();
}

void gfx_Wait(void) {
    lib.gfz_Wait();
}

void gfx_SwapDraw(void) {
    lib.gfz_SwapDraw();
}

uint8_t gfx_GetDraw(void) {
    return lib.gfz_GetDraw();
}

void gfx_SetDraw(uint8_t location) {
    lib.gfz_SetDraw(location);
}

//------------------------------------------------------------------------------
// Clipping
//------------------------------------------------------------------------------

void gfx_SetClipRegion(ti_int xmin, ti_int ymin, ti_int xmax, ti_int ymax) {
    lib.gfz_SetClipRegion(xmin, ymin, xmax, ymax);
}

bool gfx_GetClipRegion(gfx_region_t *region) {
    return lib.gfz_GetClipRegion(reinterpret_cast<gfz_region_t*>(region));
}

//------------------------------------------------------------------------------
// Blitting
//------------------------------------------------------------------------------

void gfx_Blit(gfx_location_t src) {
    lib.gfz_Blit(static_cast<gfz_location_t>(src));
}

void gfx_BlitLines(gfx_location_t src, uint8_t y_loc, uint8_t num_lines) {
    const uint8_t *src_buf = gfx_vram;
    uint8_t *dst_buf = gfx_vram + (GFX_LCD_WIDTH * GFX_LCD_HEIGHT);
    if (src) {
        src_buf = gfx_vram + (GFX_LCD_WIDTH * GFX_LCD_HEIGHT);
        dst_buf = gfx_vram;
    }
    src_buf += y_loc * GFX_LCD_WIDTH;
    dst_buf += y_loc * GFX_LCD_WIDTH;
    memcpy(dst_buf, src_buf, num_lines * GFX_LCD_WIDTH);
}

void gfx_BlitRectangle(
    gfx_location_t src,
    uint24_t x,
    uint8_t y,
    uint24_t width,
    uint24_t height
) {
    const uint8_t *src_buf = gfx_vram;
    uint8_t *dst_buf = gfx_vram + (GFX_LCD_WIDTH * GFX_LCD_HEIGHT);
    if (src) {
        src_buf = gfx_vram + (GFX_LCD_WIDTH * GFX_LCD_HEIGHT);
        dst_buf = gfx_vram;
    }
    src_buf += y + (x * GFX_LCD_WIDTH);
    dst_buf += y + (x * GFX_LCD_WIDTH);
    for (uint24_t y_cord = 0; y_cord < height; y_cord++) {
        memcpy(dst_buf, src_buf, width);
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

void gfx_CopyRectangle(
    gfx_location_t src, gfx_location_t dst,
    uint24_t src_x, uint8_t src_y,
    uint24_t dst_x, uint8_t dst_y,
    uint24_t width, uint8_t height
) {
    const uint8_t* src_buf = (src == gfx_screen) ?
        (uint8_t*)RAM_ADDRESS(lcd_UpBase) : (uint8_t*)RAM_ADDRESS(lcd_LpBase);
    uint8_t* dst_buf = (dst == gfx_screen) ?
        (uint8_t*)RAM_ADDRESS(lcd_UpBase) : (uint8_t*)RAM_ADDRESS(lcd_LpBase);
    const bool buf_overlap = (src_buf == dst_buf) ? true : false;
    src_buf += src_x + (src_y * GFX_LCD_WIDTH);
    dst_buf += dst_x + (dst_y * GFX_LCD_WIDTH);
    const uint24_t jump = GFX_LCD_WIDTH - width;
    if (buf_overlap == true) {
        for (uint24_t y_cord = 0; y_cord < height; y_cord++) {
            memmove(dst_buf, src_buf, width);
            src_buf += jump;
            dst_buf += jump;
        }
        return;
    }
    for (uint24_t y_cord = 0; y_cord < height; y_cord++) {
        memcpy(dst_buf, src_buf, width);
        src_buf += jump;
        dst_buf += jump;
    }
}

//------------------------------------------------------------------------------
// Screen Shifting
//------------------------------------------------------------------------------

void gfx_ShiftDown(uint8_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + ((lib.ClipYMin + (int8_t)pixels) * GFX_LCD_WIDTH);
    const size_t copySize = lib.ClipXMax - lib.ClipXMin;
    ti_int y0 = lib.ClipYMin + (int8_t)pixels;
    ti_int y1 = lib.ClipYMax;
    src_buf += pixels * GFX_LCD_WIDTH;
    dst_buf += pixels * GFX_LCD_WIDTH;
    for (ti_int y = y0; y < y1; y++) {
        memcpy(dst_buf, src_buf, copySize);
        src_buf -= GFX_LCD_WIDTH;
        dst_buf -= GFX_LCD_WIDTH;
    }
}

void gfx_ShiftUp(uint8_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + ((lib.ClipYMin - (int8_t)pixels) * GFX_LCD_WIDTH);
    const ti_int copySize = lib.ClipXMax - lib.ClipXMin - (ti_int)pixels;
    if (copySize <= 0) { return; }
    ti_int y0 = lib.ClipYMin + (int8_t)pixels;
    ti_int y1 = lib.ClipYMax;
    src_buf -= pixels * GFX_LCD_WIDTH;
    dst_buf -= pixels * GFX_LCD_WIDTH;
    for (ti_int y = y0; y < y1; y++) {
        memcpy(dst_buf, src_buf, (size_t)copySize);
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

void gfx_ShiftLeft(uint24_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.ClipXMin - (ti_int)pixels) + (lib.ClipYMin * GFX_LCD_WIDTH);
    const ti_int copySize = lib.ClipXMax - lib.ClipXMin - (ti_int)pixels;
    if (copySize <= 0) { return; }
    ti_int y0 = lib.ClipYMin;
    ti_int y1 = lib.ClipYMax;
    for (ti_int y = y0; y < y1; y++) {
        memmove(dst_buf, src_buf, (size_t)copySize); // memcpy would be UB
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

void gfx_ShiftRight(uint24_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.ClipXMin + (ti_int)pixels) + (lib.ClipYMin * GFX_LCD_WIDTH);
    const size_t copySize = lib.ClipXMax - lib.ClipXMin - pixels;
    ti_int y0 = lib.ClipYMin;
    ti_int y1 = lib.ClipYMax;
    for (ti_int y = y0; y < y1; y++) {
        memmove(dst_buf, src_buf, copySize); // memcpy would be UB
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

//------------------------------------------------------------------------------
// Pixel Functions
//------------------------------------------------------------------------------

template<>
uint8_t GraphX::gfz_GetPixel(ti_unsigned_int x, uint8_t y) {
    return ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(x & 0xFFFF) + ((uint24_t)y * GFX_LCD_WIDTH)];
}

template<>
void GraphX::gfz_SetPixel(ti_unsigned_int x, uint8_t y) {
    if (x < GFX_LCD_WIDTH && y < GFX_LCD_HEIGHT) {
        gfz_SetPixel_NoClip(x, y, Color);
    }
}

void gfx_SetPixel(uint24_t x, uint8_t y) {
    if (x < GFX_LCD_WIDTH && y < GFX_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint24_t)x + (y * GFX_LCD_WIDTH)] = lib.Color;
    }
}

uint8_t gfx_GetPixel(uint24_t x, uint8_t y) {
    return ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(x & 0xFFFF) + ((uint24_t)y * GFX_LCD_WIDTH)];
}

//------------------------------------------------------------------------------
// Screen Filling
//------------------------------------------------------------------------------

void gfx_FillScreen(uint8_t index) {
    lib.gfz_FillScreen(index);
}

void gfx_ZeroScreen(void) {
    lib.gfz_ZeroScreen();
}

//------------------------------------------------------------------------------
// Horizontal Lines
//------------------------------------------------------------------------------

void gfx_HorizLine_NoClip(uint24_t x, uint8_t y, uint24_t length) { //x start, y postion, x length
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint24_t)y * GFX_LCD_WIDTH));
    memset(fill, lib.Color, length);
}

void gfx_HorizLine(ti_int x, ti_int y, ti_int length) {
    if (y < lib.ClipYMin || y >= lib.ClipYMax || x >= lib.ClipXMax) {
        return;
    }
    if (x < lib.ClipXMin) {
        length -= lib.ClipXMin - x;
        x = 0;
    }
    length = (x + length > lib.ClipXMax) ? (lib.ClipXMax - x) : length;
    if (length <= 0) {
        return;
    }
    gfx_HorizLine_NoClip(x, y, length);
}

//------------------------------------------------------------------------------
// Vertical Lines
//------------------------------------------------------------------------------

void gfx_VertLine_NoClip(uint24_t x, uint8_t y, uint24_t length) { //x postion, y start, y length
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint24_t)y * GFX_LCD_WIDTH));
    for (uint24_t y_cord = 0; y_cord < length; y_cord++) {
        *fill = lib.Color;
        fill += GFX_LCD_WIDTH;
    }
}

void gfx_VertLine(ti_int x, ti_int y, ti_int length) {
    if (x < lib.ClipXMin || x >= lib.ClipXMax || y >= lib.ClipYMax) {
        return;
    }
    if (y < lib.ClipYMin) {
        length -= lib.ClipYMin - y;
        y = 0;
    }
    length = (y + length > lib.ClipYMax) ? (lib.ClipYMax - y) : length;
    if (length <= 0) {
        return;
    }
    gfx_VertLine_NoClip(x, y, length);
}

//------------------------------------------------------------------------------
// Filled Rectangle
//------------------------------------------------------------------------------

void gfx_FillRectangle_NoClip(uint24_t x, uint8_t y, uint24_t width, uint8_t height) {
    if (width == 0 || height == 0) {
        return;
    }
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint24_t)y * GFX_LCD_WIDTH));
    for (uint8_t y_cord = 0; y_cord < height; y_cord++) {
        memset(fill, lib.Color, width);
        fill += GFX_LCD_WIDTH;
    }
}

void gfx_FillRectangle(ti_int x, ti_int y, ti_int width, ti_int height) {
    if (x < lib.ClipXMin) {
        width -= lib.ClipXMin - x;
        x = 0;
    }
    if (y < lib.ClipYMin) {
        height -= lib.ClipYMin - y;
        y = 0;
    }
    width = (x + width > lib.ClipXMax) ? (lib.ClipXMax - x) : width;
    height = (y + height > lib.ClipYMax) ? (lib.ClipYMax - y) : height;
    if (width <= 0 || height <= 0) {
        return;
    }
    gfx_FillRectangle_NoClip(x, y, width, height);
}

//------------------------------------------------------------------------------
// Rectangle Outline
//------------------------------------------------------------------------------

void gfx_Rectangle_NoClip(uint24_t x, uint8_t y, uint24_t width, uint8_t height) {
    gfx_HorizLine_NoClip(x            , y             , width );
    gfx_HorizLine_NoClip(x            , y + height - 1, width );
    gfx_VertLine_NoClip (x            , y             , height);
    gfx_VertLine_NoClip (x + width - 1, y             , height);
}

void gfx_Rectangle(ti_int x, ti_int y, ti_int width, ti_int height) {
    gfx_HorizLine(x            , y             , width );
    gfx_HorizLine(x            , y + height - 1, width );
    gfx_VertLine (x            , y             , height);
    gfx_VertLine (x + width - 1, y             , height);
}

//------------------------------------------------------------------------------
// Text and Fonts
//------------------------------------------------------------------------------

void gfx_SetMonospaceFont(uint8_t spacing) {
    lib.MonospaceFont = spacing;
}

uint8_t gfx_SetFontHeight(uint8_t height) {
    if (height <= 0 || height > lib.Maximum_Font_Height) {
        lib.print_warning("font height out of range: %d", height);
    }
    uint8_t temp = lib.FontHeight;
    lib.FontHeight = height;
    return temp;
}

void gfx_SetTextConfig(uint8_t config) {
    lib.PrintChar_Clip = config;
}

void gfx_SetTextXY(ti_int x, ti_int y) {
    lib.TextXPos = x;
    lib.TextYPos = y;
    lib.test_state();
}

ti_int gfx_GetTextY(void) {
    return lib.TextYPos;
}

ti_int gfx_GetTextX(void) {
    return lib.TextXPos;
}

void gfx_SetFontSpacing(const uint8_t *data) {
    lib.CharSpacing = (data == NULL) ? gfx_DefaultCharSpacing : data;
}

uint8_t *gfx_SetFontData(const uint8_t *data) {
    uint8_t* temp = (uint8_t*)lib.TextData;
    lib.TextData = (data == NULL) ? gfx_DefaultTextData : data;
    return temp;
}

uint8_t *gfx_SetCharData(uint8_t index, const uint8_t *data) {
    return (uint8_t*)memcpy(&((uint8_t*)lib.TextData)[index * 8], data, 8 * 8);
}

uint24_t gfx_GetCharWidth(const char c) {
    return (lib.MonospaceFont != 0) ? lib.MonospaceFont : lib.CharSpacing[(unsigned char)c];
}

void gfx_SetTextScale(uint8_t width_scale, uint8_t height_scale) {
    lib.TextWidthScale = width_scale;
    lib.TextHeightScale = height_scale;
    lib.test_state();
}

//------------------------------------------------------------------------------
// Character Drawing
//------------------------------------------------------------------------------

static void gfx_internal_PrintChar_NoClip(const char c, const uint8_t charWidth) {
    const uint8_t *bitImage = lib.TextData + lib.Maximum_Font_Height * (uint24_t)((unsigned char)c);
    uint8_t *fillLinePtr = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.TextXPos + (lib.TextYPos * GFX_LCD_WIDTH));

    lib.TextXPos += charWidth * lib.TextWidthScale;

    for (uint8_t y = 0; y < lib.FontHeight; y++) {
        for (uint8_t v = 0; v < lib.TextHeightScale; v++) {
            uint8_t *fillPtr = fillLinePtr;
            uint8_t b = (1 << 7);
            for (uint8_t x = 0; x < charWidth; x++) {
                const uint8_t fillColor = *bitImage & b ? lib.Text_FG_Color : lib.Text_BG_Color;
                b >>= 1;

                if (fillColor == lib.Text_TP_Color) {
                    fillPtr += lib.TextWidthScale;
                    continue;
                }
                for (uint8_t u = 0; u < lib.TextWidthScale; u++) {
                    *fillPtr = fillColor;
                    fillPtr++;
                }
            }
            fillLinePtr += GFX_LCD_WIDTH;
        }
        bitImage++;
    }
}

void gfx_PrintChar(const char c) {
    const uint8_t charWidth = gfx_GetCharWidth(c);
    const uint8_t textSizeX = charWidth * lib.TextWidthScale;
    const uint8_t textSizeY = lib.Maximum_Font_Height * lib.TextHeightScale;
    if (
        lib.PrintChar_Clip == gfx_text_noclip ||
        /* Otherwise, if clipping is enabled */
        lib.TextXPos >= lib.ClipXMin || lib.TextYPos >= lib.ClipYMin ||
        lib.TextXPos + textSizeX <= lib.ClipXMax ||
        lib.TextYPos + textSizeY <= lib.ClipYMax
    ) {
        gfx_internal_PrintChar_NoClip(c, charWidth);
        return;
    }
    const uint8_t *bitImage = lib.TextData + lib.Maximum_Font_Height * (uint24_t)((unsigned char)c);
    uint8_t *fillLinePtr = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.TextXPos + (lib.TextYPos * GFX_LCD_WIDTH));
    lib.TextXPos += charWidth * lib.TextWidthScale;

    for (uint8_t y = 0; y < lib.FontHeight; y++) {
        for (uint8_t v = 0; v < lib.TextHeightScale; v++) {
            uint8_t *fillPtr = fillLinePtr;
            uint8_t b = (1 << 7);
            for (uint8_t x = 0; x < charWidth; x++) {
                const uint8_t fillColor = *bitImage & b ? lib.Text_FG_Color : lib.Text_BG_Color;
                b >>= 1;
                if (fillColor == lib.Text_TP_Color) {
                    fillPtr += lib.TextWidthScale;
                    continue;
                }
                for (uint8_t u = 0; u < lib.TextWidthScale; u++) {
                    if (
                        fillPtr >= (uint8_t*)RAM_ADDRESS(CurrentBuffer) &&
                        fillPtr < (uint8_t*)RAM_ADDRESS(CurrentBuffer + GFX_LCD_WIDTH * GFX_LCD_HEIGHT)
                    ) {
                        *fillPtr = fillColor;
                    }
                    fillPtr++;
                }
            }
            fillLinePtr += GFX_LCD_WIDTH;
        }
        bitImage++;
    }
}

//------------------------------------------------------------------------------
// String Functions
//------------------------------------------------------------------------------

void gfx_PrintUInt(uint24_t n, uint8_t length) {
    if (length < 1 || length > 8) {
        printf("gfx_Print(U)Int: invalid length %u\n", length);
        if (length > 8) {
            length = 8;
        }
        if (length < 1) {
            length = 1;
        }
    }
    uint8_t digit_count = 0;
    {
        uint24_t n_temp = n;
        while (n_temp != 0) {
            n_temp /= 10;
            digit_count++;
        }
    }
    for (uint8_t l = digit_count; l < length; l++) {
        gfx_PrintChar('0');
    }
    uint8_t digit;
    switch (digit_count) {
        case 8: // 10'000'000 <= n <= 16'777'215
            gfx_PrintChar('1');
        case 7:
            digit = ((n / 1000000) % 10) + '0';
            gfx_PrintChar(digit);
        case 6:
            digit = ((n / 100000) % 10) + '0';
            gfx_PrintChar(digit);
        case 5:
            digit = ((n / 10000) % 10) + '0';
            gfx_PrintChar(digit);
        case 4:
            digit = ((n / 1000) % 10) + '0';
            gfx_PrintChar(digit);
        case 3:
            digit = ((n / 100) % 10) + '0';
            gfx_PrintChar(digit);
        case 2:
            digit = ((n / 10) % 10) + '0';
            gfx_PrintChar(digit);
        case 1:
            digit = (n % 10) + '0';
            gfx_PrintChar(digit);
        case 0:
        return;
    }
}

void gfx_PrintInt(int24_t n, uint8_t length) {
    if (n >= 0) {
        gfx_PrintUInt(n, length);
        return;
    }
    gfx_PrintChar('-');
    if (length > 1) {
        length--;
    }
    gfx_PrintUInt(-n, length);
}

void gfx_PrintString(const char *string) {
    while (*string != '\0') {
        gfx_PrintChar(*string);
        string++;
    }
}

void gfx_PrintStringXY(const char *string, ti_int x, ti_int y) {
    gfx_SetTextXY(x, y);
    gfx_PrintString(string);
}

uint24_t gfx_GetStringWidth(const char *string) {
    uint24_t len = 0;
    while (*string != '\0') {
        len += gfx_GetCharWidth(*string);
        string++;
    }
    return len * lib.TextWidthScale;
}

//------------------------------------------------------------------------------
// Unclipped Lines
//------------------------------------------------------------------------------

void gfx_Line_NoClip(uint24_t x0, uint8_t y0, uint24_t x1, uint8_t y1) {
    lib.gfz_Line_NoClip(x0, y0, x1, y1);
}

//------------------------------------------------------------------------------
// Clipped Lines
//------------------------------------------------------------------------------

void gfx_Line(ti_int x0, ti_int y0, ti_int x1, ti_int y1) {
    lib.gfz_Line(x0, y0, x1, y1);
}

//------------------------------------------------------------------------------
// Circles
//------------------------------------------------------------------------------

// https://zingl.github.io/bresenham.html
/** @todo make function pixel perfect */
void gfx_Circle(
    const ti_int x, const ti_int y, const uint24_t radius
) {
    ti_int r = radius;

    ti_int x_pos = -r;
    ti_int y_pos = 0;
    ti_int err = 2 - 2 * r;
    do {
        lib.gfz_SetPixel_RegionClip(x - x_pos, y + y_pos, lib.Color);
        lib.gfz_SetPixel_RegionClip(x - y_pos, y - x_pos, lib.Color);
        lib.gfz_SetPixel_RegionClip(x + x_pos, y - y_pos, lib.Color);
        lib.gfz_SetPixel_RegionClip(x + y_pos, y + x_pos, lib.Color);
        r = err;
        if (r <= y_pos) {
            err += ++y_pos * 2 + 1;
        }
        if (r > x_pos || err > y_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos < 0);
}

// https://zingl.github.io/bresenham.html
/** @todo make function pixel perfect */
void gfx_FillCircle(
    const ti_int x, const ti_int y, const uint24_t radius
) {
    ti_int r = radius;

    ti_int x_pos = -r;
    ti_int y_pos = 0;
    ti_int err = 2 - 2 * r;
    do {
        gfx_HorizLine(x + x_pos, y - y_pos, -x_pos * 2 + 1);
        gfx_HorizLine(x + x_pos, y + y_pos, -x_pos * 2 + 1);
        r = err;
        if (r <= y_pos) {
            y_pos++;
            err += y_pos * 2 + 1;
        }
        if (r > x_pos || err > y_pos) {
            x_pos++;
            err += x_pos * 2 + 1;
        }
    } while (x_pos < 0);
}

// https://zingl.github.io/bresenham.html
/** @todo make function pixel perfect */
void gfx_FillCircle_NoClip(
    const uint24_t x, const uint8_t y, const uint24_t radius
) {
    ti_int r = radius;

    ti_int x_pos = -r;
    ti_int y_pos = 0;
    ti_int err = 2 - 2 * r;
    do {
        gfx_HorizLine_NoClip(x + x_pos, (uint8_t)(y - y_pos), -x_pos * 2 + 1);
        gfx_HorizLine_NoClip(x + x_pos, (uint8_t)(y + y_pos), -x_pos * 2 + 1);
        r = err;
        if (r <= y_pos) {
            y_pos++;
            err += y_pos * 2 + 1;
        }
        if (r > x_pos || err > y_pos) {
            x_pos++;
            err += x_pos * 2 + 1;
        }
    } while (x_pos < 0);
}

//------------------------------------------------------------------------------
// Ellipses
//------------------------------------------------------------------------------

static void gfx_internal_Ellipse_dual_point(
    ti_int x, ti_int y, ti_int xc, ti_int yc
) {
    lib.gfz_SetPixel_RegionClip(x - xc, y - yc, lib.Color);
    lib.gfz_SetPixel_RegionClip(x + xc, y - yc, lib.Color);
    lib.gfz_SetPixel_RegionClip(x - xc, y + yc, lib.Color);
    lib.gfz_SetPixel_RegionClip(x + xc, y + yc, lib.Color);
}

static void gfx_internal_Ellipse_dual_point_NoClip(
    ti_int x, ti_int y, ti_int xc, ti_int yc
) {
    lib.gfz_SetPixel_NoClip(x - xc, y - yc, lib.Color);
    lib.gfz_SetPixel_NoClip(x + xc, y - yc, lib.Color);
    lib.gfz_SetPixel_NoClip(x - xc, y + yc, lib.Color);
    lib.gfz_SetPixel_NoClip(x + xc, y + yc, lib.Color);
}

static void gfx_internal_Ellipse_dual_line(
    ti_int x, ti_int y, ti_int xc, ti_int yc
) {
    gfx_HorizLine(x - xc, y - yc, 2 * xc);
    gfx_HorizLine(x - xc, y + yc, 2 * xc);
}

static void gfx_internal_Ellipse_dual_line_NoClip(
    ti_int x, ti_int y, ti_int xc, ti_int yc
) {
    gfx_HorizLine_NoClip(x - xc, (uint8_t)(y - yc), 2 * xc);
    gfx_HorizLine_NoClip(x - xc, (uint8_t)(y + yc), 2 * xc);
}

// Derived from graphx.asm
static void gfx_internal_Ellipse(
    int24_t x, int24_t y, uint24_t a, uint24_t b,
    void (*plot_function)(int24_t, int24_t, int24_t, int24_t)
) {
    int24_t a2 = a * a;
    int24_t fa2 = 4 * a2;
    if (a == 0 || b == 0) {
        return;
    }
    int24_t yc = b;
    int24_t sigma_add_1 = fa2 * (1 - b);
    int24_t b2 = b * b;
    int24_t fb2 = 4 * b2;
    int24_t xc = 0;
    int24_t sigma = 2 * b2 + a2 * (1 - 2 * b);
    int24_t sigma_add_2 = fb2 * (1 - a);

    while(b2 * xc <= a2 * yc) { /* .main_loop1 */
        (*plot_function)(x, y, xc, yc);
        if (sigma >= 0) {
            sigma += sigma_add_1;
            sigma_add_1 += fa2;
            yc--;
        }

        sigma += b2 * (4 * xc + 6);
        xc++;
    }

    xc = a;
    yc = 0;
    sigma = 2 * a2 + b2 * (1 - 2 * a);

    while(a2 * yc <= b2 * xc) {
        (*plot_function)(x, y, xc, yc);
        if (sigma >= 0) {
            sigma += sigma_add_2;
            sigma_add_2 += fb2;
            xc--;
        }

        sigma += a2 * (4 * yc + 6);
        yc++;
    }
}

void gfx_Ellipse(int24_t x, int24_t y, uint24_t a, uint24_t b) {
    gfx_internal_Ellipse(
        x, y, a, b,
        gfx_internal_Ellipse_dual_point
    );
}

void gfx_Ellipse_NoClip(uint24_t x, uint24_t y, uint8_t a, uint8_t b) {
    gfx_internal_Ellipse(
        (int24_t)x, (int24_t)y, (uint24_t)a, (uint24_t)b,
        gfx_internal_Ellipse_dual_point_NoClip
    );
}

void gfx_FillEllipse(int24_t x, int24_t y, uint24_t a, uint24_t b) {
    gfx_internal_Ellipse(
        x, y, a, b,
        gfx_internal_Ellipse_dual_line
    );
}

void gfx_FillEllipse_NoClip(uint24_t x, uint24_t y, uint8_t a, uint8_t b) {
    gfx_internal_Ellipse(
        (int24_t)x, (int24_t)y, (uint24_t)a, (uint24_t)b,
        gfx_internal_Ellipse_dual_line_NoClip
    );
}

//------------------------------------------------------------------------------
// Triangles
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Triangles
//------------------------------------------------------------------------------

// y2 >= y1 >= y0
static void gfx_internal_triangle_sort(
    int24_t* x0, int24_t* y0,
    int24_t* x1, int24_t* y1,
    int24_t* x2, int24_t* y2
) {
    #define triangle_swap(a, b) \
        { \
            int24_t temp = *(a); \
            *(a) = *(b); \
            *(b) = temp; \
        }

    if (*y0 > *y1) {
        triangle_swap(x0, x1);
        triangle_swap(y0, y1);
    }
    if (*y1 > *y2) {
        triangle_swap(x1, x2);
        triangle_swap(y1, y2);
    }
    if (*y0 > *y1) {
        triangle_swap(x0, x1);
        triangle_swap(y0, y1);
    }

    #undef triangle_swap
}

void gfx_FillTriangle(
    int24_t x0, int24_t y0,
    int24_t x1, int24_t y1,
    int24_t x2, int24_t y2
) {
    gfx_internal_triangle_sort(
        &x0, &y0,
        &x1, &y1,
        &x2, &y2
    );
    if (y0 == y2) { // Degenerate Triangle
        int24_t x_min = (x0 < x1) ? x0 : x1;
        x_min = (x_min < x2) ? x_min : x2;
        int24_t x_max = (x0 > x1) ? x0 : x1;
        x_max = (x_max > x2) ? x_max : x2;
        gfx_HorizLine(x_min, y0, x_max - x_min + 1);
        return;
    }
    int24_t sa = 0;
    int24_t sb = 0;
    int24_t dx01 = x1 - x0;
    int24_t dx02 = x2 - x0;
    int24_t dy01 = y1 - y0;
    int24_t dy02 = y2 - y0;
    int24_t dx12 = x2 - x1;
    int24_t dy12 = y2 - y1;
    int24_t last;
    if (y1 == y2) {
        last = y1;
    } else {
        last = y1 - 1;
    }
    int24_t y;
    for (y = y0; y <= last; y++) {
        int24_t a = x0 + sa / dy01;
        int24_t b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (b < a) { // Swap
            int24_t temp = a;
            a = b;
            b = temp;
        }
        gfx_HorizLine(a, y, b - a + 1);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y <= y2; y++) {
        int24_t a = x1 + sa / dy12;
        int24_t b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (b < a) { // Swap
            int24_t temp = a;
            a = b;
            b = temp;
        }
        gfx_HorizLine(a, y, b - a + 1);
    }
}

void gfx_FillTriangle_NoClip(
    int24_t x0, int24_t y0,
    int24_t x1, int24_t y1,
    int24_t x2, int24_t y2
) {
    gfx_internal_triangle_sort(
        &x0, &y0,
        &x1, &y1,
        &x2, &y2
    );
    if (y0 == y2) { // Degenerate Triangle
        int24_t x_min = (x0 < x1) ? x0 : x1;
        x_min = (x_min < x2) ? x_min : x2;
        int24_t x_max = (x0 > x1) ? x0 : x1;
        x_max = (x_max > x2) ? x_max : x2;
        gfx_HorizLine_NoClip(x_min, (uint8_t)y0, x_max - x_min + 1);
        return;
    }
    int24_t sa = 0;
    int24_t sb = 0;
    int24_t dx01 = x1 - x0;
    int24_t dx02 = x2 - x0;
    int24_t dy01 = y1 - y0;
    int24_t dy02 = y2 - y0;
    int24_t dx12 = x2 - x1;
    int24_t dy12 = y2 - y1;
    int24_t last;
    if (y1 == y2) {
        last = y1;
    } else {
        last = y1 - 1;
    }
    int24_t y;
    for (y = y0; y <= last; y++) {
        int24_t a = x0 + sa / dy01;
        int24_t b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (b < a) { // Swap
            int24_t temp = a;
            a = b;
            b = temp;
        }
        gfx_HorizLine_NoClip(a, (uint8_t)y, b - a + 1);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y <= y2; y++) {
        int24_t a = x1 + sa / dy12;
        int24_t b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (b < a) { // Swap
            int24_t temp = a;
            a = b;
            b = temp;
        }
        gfx_HorizLine_NoClip(a, (uint8_t)y, b - a + 1);
    }
}

//------------------------------------------------------------------------------
// Unfilled Polygons
//------------------------------------------------------------------------------

void gfx_Polygon(const int24_t *points, size_t num_points) {
    if (num_points < 2) {
        return;
    }
    gfx_Line(
        points[num_points * 2 - 2], points[num_points * 2 - 1],
        points[0], points[1]
    );
    for (size_t i = 2; i < 2 * num_points; i += 2) {
        gfx_Line(
            points[i + 0], points[i + 1],
            points[i + 2], points[i + 3]
        );
    }
}

void gfx_Polygon_NoClip(const int24_t *points, size_t num_points) {
    if (num_points < 2) {
        return;
    }
    gfx_Line(
        points[num_points * 2 - 2], points[num_points * 2 - 1],
        points[0], points[1]
    );
    for (size_t i = 2; i < 2 * num_points; i += 2) {
        gfx_Line(
            points[i + 0], points[i + 1],
            points[i + 2], points[i + 3]
        );
    }
}

//------------------------------------------------------------------------------
// Floodfill
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Sprite Utilities
//------------------------------------------------------------------------------

gfx_sprite_t *gfx_AllocSprite(
    uint8_t width,
    uint8_t height,
    void *(*malloc_routine)(size_t)
) {
    return reinterpret_cast<gfx_sprite_t*>(lib.gfz_AllocSprite(width, height, malloc_routine));
}

gfx_sprite_t *gfx_GetSprite(gfx_sprite_t *sprite_buffer, int24_t x, int24_t y) {
    uint8_t* dst_buf = sprite_buffer->data;
    const uint8_t* src_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFX_LCD_WIDTH);

    for (uint8_t y_cord = 0; y_cord < sprite_buffer->height; y_cord++) {
        memcpy(dst_buf, src_buf, sprite_buffer->width);
        src_buf += GFX_LCD_WIDTH;
        dst_buf += sprite_buffer->width;
    }
    return sprite_buffer;
}

gfx_sprite_t *gfx_GetSpriteChar(char c) {
    const uint8_t *bitImage = lib.TextData + lib.Maximum_Font_Height * (uint24_t)((unsigned char)c);
    gfx_sprite_t * const tempSpriteChar = reinterpret_cast<gfx_sprite_t*>(&lib.TmpCharSprite);
    uint8_t *fillPtr = tempSpriteChar->data;

    const uint8_t CharWidth = gfx_GetCharWidth(c);

    for (uint8_t y = 0; y < lib.FontHeight; y++) {
        uint8_t b = 1;
        uint8_t x = 0;
        for (; x < CharWidth; x++) {
            *fillPtr = (*bitImage & b) ? lib.Text_FG_Color : lib.Text_BG_Color;
            fillPtr++;
            b <<= 1;
        }
        for (; x < lib.Maximum_Font_Width; x++) {
            *fillPtr = lib.Text_BG_Color;
            fillPtr++;
        }
        bitImage++;
    }
    memset(fillPtr, lib.Text_BG_Color,
        (lib.Maximum_Font_Height - lib.FontHeight) * lib.Maximum_Font_Width
    );
    return tempSpriteChar;
}

//------------------------------------------------------------------------------
// Sprite Othogonal Transformations
//------------------------------------------------------------------------------

gfx_sprite_t *gfx_FlipSpriteY(const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out) {
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;
    const uint8_t* src_buf = sprite_in->data + (sprite_in->width - 1);
    uint8_t* dst_buf = sprite_out->data;
    const uint24_t src_jump = (2 * sprite_in->width);
    for (uint8_t y = 0; y < sprite_in->height; y++) {
        for (uint8_t x = 0; x < sprite_in->width; x++) {
            *dst_buf = *src_buf;
            src_buf--;
            dst_buf++;
        }
        src_buf += src_jump;
    }
    return sprite_out;
}

gfx_sprite_t *gfx_FlipSpriteX(const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out) {
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;
    const uint8_t* src_buf = sprite_in->data + (sprite_in->width * (sprite_in->height - 1));
    uint8_t* dst_buf = sprite_out->data;
    for (uint8_t y = 0; y < sprite_in->height; y++) {
        memcpy(dst_buf, src_buf, sprite_in->width);
        src_buf -= sprite_in->width;
        dst_buf += sprite_in->width;
    }
    return sprite_out;
}

gfx_sprite_t *gfx_RotateSpriteC(const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out) {
    sprite_out->height = sprite_in->width;
    sprite_out->width = sprite_in->height;
    const uint8_t* src_ptr = (const uint8_t*)&sprite_in->data;
    for (uint8_t x = 0; x < sprite_in->width; x++) {
        for (uint8_t y = 0; y < sprite_in->height; y++) {
            sprite_out->data[(sprite_in->width - x - 1) + y * sprite_in->width] = *src_ptr;
            src_ptr++;
        }
    }
    return sprite_out;
}

gfx_sprite_t *gfx_RotateSpriteCC(const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out) {
    sprite_out->height = sprite_in->width;
    sprite_out->width = sprite_in->height;
    const uint8_t* src_ptr = (const uint8_t*)&sprite_in->data;
    for (uint8_t x = 0; x < sprite_in->width; x++) {
        for (uint8_t y = 0; y < sprite_in->height; y++) {
            sprite_out->data[x + (sprite_in->height - y - 1) * sprite_in->width] = *src_ptr;
            src_ptr++;
        }
    }
    return sprite_out;
}

gfx_sprite_t *gfx_RotateSpriteHalf(const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out) {
    const uint8_t* src_buf = sprite_in->data;
    uint8_t* dst_buf = sprite_out->data + (sprite_in->width * sprite_in->height);
    while (dst_buf != sprite_out->data) {
        dst_buf--;
        *dst_buf = *src_buf;
        src_buf++;
    }
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;
    return sprite_out;
}

//------------------------------------------------------------------------------
// Sprite Drawing
//------------------------------------------------------------------------------

/* gfx_Sprite */

void gfx_Sprite(const gfx_sprite_t *sprite, int24_t x, int24_t y) {
    if (
        x >= lib.ClipXMax || y >= lib.ClipYMax ||
        sprite->width == 0 || sprite->height == 0 ||
        x + sprite->width < lib.ClipXMin ||
        y + sprite->height < lib.ClipYMin
    ) {
        return;
    }
    if (
        x >= lib.ClipXMin &&
        y >= lib.ClipYMin &&
        x + sprite->width <= lib.ClipXMax &&
        y + sprite->height <= lib.ClipYMax
    ) {
        gfx_Sprite_NoClip(sprite, (uint24_t)x, (uint8_t)y);
        return;
    }
    const uint8_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint8_t min_clipY = (y < lib.ClipYMin) ? (lib.ClipYMin - y) : 0;
    const uint8_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;
    const uint8_t max_clipY = ((y + sprite->height) > lib.ClipYMax) ? ((y + sprite->height) - lib.ClipYMax) : 0;

    uint8_t sizeX = sprite->width - (min_clipX + max_clipX);
    uint8_t sizeY = sprite->height - (min_clipY + max_clipY);

    const uint8_t* src_buf = sprite->data + min_clipX + (min_clipY * sprite->width);

    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + min_clipX) + ((y + min_clipY) * GFX_LCD_WIDTH);

    for (uint8_t y_cord = 0; y_cord < sizeY; y_cord++) {
        memcpy(dst_buf, src_buf, sizeX);
        src_buf += sprite->width;
        dst_buf += GFX_LCD_WIDTH;
    }
}

/* gfx_TransparentSprite */

void gfx_TransparentSprite(const gfx_sprite_t *sprite, int24_t x, int24_t y) {
    if (
        x >= lib.ClipXMax || y >= lib.ClipYMax ||
        sprite->width == 0 || sprite->height == 0 ||
        x + sprite->width < lib.ClipXMin ||
        y + sprite->height < lib.ClipYMin
    ) {
        return;
    }
    if (
        x >= lib.ClipXMin &&
        y >= lib.ClipYMin &&
        x + sprite->width <= lib.ClipXMax &&
        y + sprite->height <= lib.ClipYMax
    ) {
        gfx_TransparentSprite_NoClip(sprite, (uint24_t)x, (uint8_t)y);
        return;
    }
    const uint8_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint8_t min_clipY = (y < lib.ClipYMin) ? (lib.ClipYMin - y) : 0;
    const uint8_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;
    const uint8_t max_clipY = ((y + sprite->height) > lib.ClipYMax) ? ((y + sprite->height) - lib.ClipYMax) : 0;

    uint8_t sizeX = sprite->width - (min_clipX + max_clipX);
    uint8_t sizeY = sprite->height - (min_clipY + max_clipY);

    const uint8_t* src_buf = sprite->data + min_clipX + (min_clipY * sprite->width);
    const uint8_t clip_jumpX = min_clipX + max_clipX;

    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + min_clipX) + ((y + min_clipY) * GFX_LCD_WIDTH);
    const uint24_t dst_jump = GFX_LCD_WIDTH - sizeX;

    for (uint8_t y_cord = 0; y_cord < sizeY; y_cord++) {
        for (uint8_t x_cord = 0; x_cord < sizeX; x_cord++) {
            *dst_buf = (*src_buf != lib.Transparent_Color) ? *src_buf : *dst_buf;
            src_buf++;
            dst_buf++;
        }
        src_buf += clip_jumpX;
        dst_buf += dst_jump;
    }
}

/* gfx_Sprite_NoClip */

void gfx_Sprite_NoClip(const gfx_sprite_t *sprite, uint24_t x, uint8_t y) {
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFX_LCD_WIDTH);

    for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
        memcpy(dst_buf, src_buf, sprite->width);
        src_buf += sprite->width;
        dst_buf += GFX_LCD_WIDTH;
    }
}

/* gfx_TransparentSprite_NoClip */

void gfx_TransparentSprite_NoClip(const gfx_sprite_t *sprite, uint24_t x, uint8_t y) {
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFX_LCD_WIDTH);
    const uint24_t dst_jump = GFX_LCD_WIDTH - sprite->width;

    for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
        for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
            *dst_buf = (*src_buf != lib.Transparent_Color) ? *src_buf : *dst_buf;
            src_buf++;
            dst_buf++;
        }
        dst_buf += dst_jump;
    }
}

//------------------------------------------------------------------------------
// Scaled Sprites
//------------------------------------------------------------------------------

void gfx_ScaledSprite_NoClip(
    const gfx_sprite_t *sprite,
    const uint24_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) {
    if (width_scale == 0 || height_scale == 0) {
        return;
    }
    const uint8_t* src_buf = sprite->data;
    uint8_t* buf_start = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFX_LCD_WIDTH);
    uint8_t* dst_buf = buf_start;
    const uint24_t dst_jump = (GFX_LCD_WIDTH * height_scale) - (sprite->width * width_scale);

    for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
        for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
            const uint8_t color = *src_buf;
            src_buf++;
            for (uint8_t u = 0; u < width_scale; u++) {
                *dst_buf = color;
                dst_buf++;
            }
        }
        dst_buf += dst_jump;
    }

    if (height_scale == 1) {
        return;
    }

    // memcpy rows
    uint8_t* dst_row = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFX_LCD_WIDTH);
    const uint24_t copySize = sprite->width * width_scale; // This could probably be uint8_t
    for (uint8_t row = 0; row < sprite->height; row++) {
        const uint8_t* const src_row = (uint8_t*)dst_row;
        dst_row += GFX_LCD_WIDTH;
        for (uint8_t y_cord = 1; y_cord < height_scale; y_cord++) {
            memcpy(dst_row, src_row, copySize);
            dst_row += GFX_LCD_WIDTH;
        }
    }
}

/* gfx_ScaledTransparentSprite_NoClip */

void gfx_ScaledTransparentSprite_NoClip(
    const gfx_sprite_t *sprite,
    const uint24_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) {
    if (width_scale == 0 || height_scale == 0) {
        return;
    }
    const uint8_t* src_buf = sprite->data;
    uint8_t* buf_start = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFX_LCD_WIDTH);
    uint8_t* dst_buf = buf_start;
    const uint24_t dst_jump = GFX_LCD_WIDTH - (sprite->width * width_scale);

    for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
        for (uint8_t v = 0; v < height_scale; v++) {
            for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
                const uint8_t color = *src_buf;
                src_buf++;
                if (color == lib.Transparent_Color) {
                    dst_buf += width_scale;
                    continue;
                }
                for (uint8_t u = 0; u < width_scale; u++) {
                    *dst_buf = color;
                    dst_buf++;
                }
            }
            dst_buf += dst_jump;
            src_buf -= sprite->width;
        }
        src_buf += sprite->width;
    }
}

gfx_sprite_t* gfx_ScaleSprite(
    const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out
) {
    uint8_t* dst_ptr = sprite_out->data;
    for (uint8_t y = 0; y < sprite_out->height; y++) {
        const uint8_t* src_ptr = sprite_in->data + ((y * sprite_in->height) / sprite_in->height) * sprite_out->width;
        for (uint8_t x = 0; x < sprite_out->width; x++) {
            *dst_ptr = *(src_ptr + (x * sprite_out->width) / sprite_in->width);
            dst_ptr++;
        }
    }
    return sprite_out;
}

//------------------------------------------------------------------------------
// Sprite Rotation
//------------------------------------------------------------------------------

/* gfx_RotateScaleSprite */

#include <math.h>

gfx_sprite_t *gfx_RotateScaleSprite(
    const gfx_sprite_t *sprite_in, gfx_sprite_t *sprite_out,
    uint8_t angle, uint8_t scale
) {
    #if 0
    const int24_t sin_val = gfx_Sin(angle) * 128 / scale;
    const int24_t cos_val = gfx_Cos(angle) * 128 / scale;
    const uint8_t size = sprite_in->width;

    sprite_out->width = sprite_in->width * scale / 64;
    sprite_out->height = sprite_in->height * scale / 64;

    int24_t dxs = sin_val * -((int24_t)(size * scale / 128));
    int24_t dxc = cos_val * -((int24_t)(size * scale / 128));

    #define dys dxs
    #define dyc dxc

    uint24_t src_index = 0;

    for (uint8_t y = 0; y < sprite_in->height; y++) {
        int24_t xs = (dxs + dyc) + (int24_t)(size * 128);
        int24_t ys = (dxc - dys) + (int24_t)(size * 128);
        for (uint8_t x = 0; x < sprite_in->width; x++) {
            xs += cos_val;
            ys -= sin_val;

            uint24_t x_write = xs / 256;
            uint24_t y_write = ys / 256;
            if (
                x_write < sprite_out->width && y_write < sprite_out->height &&
                src_index < sprite_in->width * sprite_in->height
            ) {
                sprite_out->data[x_write + sprite_out->width * y_write] = sprite_in->data[src_index];
            }
            src_index++;
        }
        dxc += cos_val;
        dxs += sin_val;
    }
    return sprite_out;

    #elif 1

    const uint8_t in_size = sprite_in->width;
    uint24_t temp_size = sprite_in->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;
    sprite_out->width = out_size;
    sprite_out->height = out_size;
    memset(sprite_out->data, lib.Transparent_Color, out_size * out_size);

    // const int24_t sin_val = gfx_Sin(angle) * 128 / scale;
    // const int24_t cos_val = gfx_Cos(angle) * 128 / scale;

    // const float sin_f = (float)sin_val / 256.0f;
    // const float cos_f = (float)cos_val / 256.0f;
    angle *= -1;

    const float sin_f = (float)sinf((float)angle * 0.0245436926f);
    const float cos_f = (float)cosf((float)angle * 0.0245436926f);

    float out_center = (float)out_size / 2.0f;
    __attribute__((__unused__)) float in_center = (float)in_size / 2.0f;
    uint8_t* dst_ptr = sprite_out->data;
    for (uint8_t y = 0; y < out_size; y++) {
        float yc = ((float)y - out_center) / (float)out_size;
        for (uint8_t x = 0; x < out_size; x++) {
            float xc = ((float)x - out_center) / (float)out_size;
            float x_pos = (xc * cos_f - yc * sin_f) + 0.5f;
            float y_pos = (yc * cos_f + xc * sin_f) + 0.5f;
            // printf("{%.2f,%.2f}", x_pos, y_pos);
            x_pos *= (float)in_size;
            y_pos *= (float)in_size;
            int24_t x_write = (int24_t)x_pos;
            int24_t y_write = (int24_t)y_pos;
            if (
                x_write < in_size && y_write < in_size &&
                x_write >= 0 && y_write >= 0
            ) {
                *dst_ptr = sprite_in->data[x_write + y_write * in_size];
            }
            dst_ptr++;
        }
        // printf("\n");
    }
    // printf("\n");
    return sprite_out;
    #else
    angle += 32;
    if (angle < 64) {
        return memcpy(sprite_out, sprite_in, sprite_in->width * sprite_in->height + sizeof(gfx_sprite_t));
    }
    if (angle < 128) {
        return gfx_RotateSpriteC(sprite_in, sprite_out);
    }
    if (angle < 192) {
        return gfx_RotateSpriteHalf(sprite_in, sprite_out);
    }
    return gfx_RotateSpriteCC(sprite_in, sprite_out);
    #endif
}

/* gfx_RotatedScaledTransparentSprite_NoClip */



/* gfx_RotatedScaledSprite_NoClip */




//------------------------------------------------------------------------------
// Tilemaps
//------------------------------------------------------------------------------

void gfx_Tilemap(const gfx_tilemap_t* tilemap, uint24_t x_offset, uint24_t y_offset) {
    // Unoptimized and overclips a bit
    uint24_t map_row = x_offset / tilemap->tile_width;
    uint24_t map_col = y_offset / tilemap->tile_height;

    const uint8_t posX_offset = x_offset % tilemap->tile_width;
    const uint8_t posY_offset = y_offset % tilemap->tile_height;

    uint24_t posX0 = tilemap->x_loc - ((posX_offset == 0) ?
        0 : tilemap->tile_width + posX_offset);
    // posX0 = (posX0 < 0) ? 0 : posX0;
    uint24_t posY = tilemap->y_loc - ((posY_offset == 0) ?
        0 : tilemap->tile_height + posY_offset);
    // posY = (posY < 0) ? 0 : posY;

    uint8_t draw_sizeX = tilemap->draw_width;
    uint8_t draw_sizeY = tilemap->draw_height;
    if (x_offset % tilemap->tile_width != 0) {
        if (map_row > 0) {
            draw_sizeX++;
            map_row--;
        } else if (posX_offset != 0) {
            posX0 += tilemap->tile_width;
        }

    }
    if (y_offset % tilemap->tile_height != 0) {
        if (map_col > 0) {
            draw_sizeY++;
            map_col--;
        } else if (posY_offset != 0) {
            posY += tilemap->tile_height;
        }
    }

    /* Debugging */ const uint24_t map_size = tilemap->width * tilemap->height;

    uint24_t map_index = map_row + (map_col * tilemap->width);
    const uint24_t map_jump = (tilemap->width - draw_sizeX);

    for (uint8_t draw_y = 0; draw_y < draw_sizeY; draw_y++) {
        uint24_t posX = posX0;
        for (uint8_t draw_x = 0; draw_x < draw_sizeX; draw_x++) {
            if (map_index >= map_size) {
                return; // Optimize this out
            }
            gfx_Sprite(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

void gfx_Tilemap_NoClip(const gfx_tilemap_t *tilemap, uint24_t x_offset, uint24_t y_offset) {
    uint24_t map_row = x_offset / tilemap->tile_width;
    uint24_t map_col = y_offset / tilemap->tile_height;

    uint24_t map_index = map_row + (map_col * tilemap->width);
    const uint24_t map_jump = (tilemap->width - tilemap->draw_width);

    uint24_t posX0 = tilemap->x_loc - (x_offset % tilemap->tile_width);
    posX0 = (posX0 < 0) ? 0 : posX0;
    uint8_t posY = tilemap->y_loc - (y_offset % tilemap->tile_height);
    posY = (posY < 0) ? 0 : posY;
    for (uint8_t draw_y = 0; draw_y < tilemap->draw_height; draw_y++) {
        uint24_t posX = posX0;
        for (uint8_t draw_x = 0; draw_x < tilemap->draw_width; draw_x++) {
            gfx_Sprite_NoClip(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

void gfx_TransparentTilemap(const gfx_tilemap_t* tilemap, uint24_t x_offset, uint24_t y_offset) {
    // Unoptimized and overclips a bit
    uint24_t map_row = x_offset / tilemap->tile_width;
    uint24_t map_col = y_offset / tilemap->tile_height;

    const uint8_t posX_offset = x_offset % tilemap->tile_width;
    const uint8_t posY_offset = y_offset % tilemap->tile_height;

    uint24_t posX0 = tilemap->x_loc - ((posX_offset == 0) ?
        0 : tilemap->tile_width + posX_offset);
    // posX0 = (posX0 < 0) ? 0 : posX0;
    uint24_t posY = tilemap->y_loc - ((posY_offset == 0) ?
        0 : tilemap->tile_height + posY_offset);
    // posY = (posY < 0) ? 0 : posY;

    uint8_t draw_sizeX = tilemap->draw_width;
    uint8_t draw_sizeY = tilemap->draw_height;
    if (x_offset % tilemap->tile_width != 0) {
        if (map_row > 0) {
            draw_sizeX++;
            map_row--;
        } else if (posX_offset != 0) {
            posX0 += tilemap->tile_width;
        }

    }
    if (y_offset % tilemap->tile_height != 0) {
        if (map_col > 0) {
            draw_sizeY++;
            map_col--;
        } else if (posY_offset != 0) {
            posY += tilemap->tile_height;
        }
    }

    /* Debugging */ const uint24_t map_size = tilemap->width * tilemap->height;

    uint24_t map_index = map_row + (map_col * tilemap->width);
    const uint24_t map_jump = (tilemap->width - draw_sizeX);

    for (uint8_t draw_y = 0; draw_y < draw_sizeY; draw_y++) {
        uint24_t posX = posX0;
        for (uint8_t draw_x = 0; draw_x < draw_sizeX; draw_x++) {
            if (map_index >= map_size) {
                return; // Optimize this out
            }
            gfx_TransparentSprite(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

void gfx_TransparentTilemap_NoClip(const gfx_tilemap_t *tilemap, uint24_t x_offset, uint24_t y_offset) {
    uint24_t map_row = x_offset / tilemap->tile_width;
    uint24_t map_col = y_offset / tilemap->tile_height;
    uint24_t map_index = map_row + (map_col * tilemap->width);
    const uint24_t map_jump = (tilemap->width - tilemap->draw_width);

    uint24_t posX0 = tilemap->x_loc - (x_offset % tilemap->tile_width);
    posX0 = (posX0 < 0) ? 0 : posX0;
    uint8_t posY = tilemap->y_loc - (y_offset % tilemap->tile_height);
    posY = (posY < 0) ? 0 : posY;
    for (uint8_t draw_y = 0; draw_y < tilemap->draw_height; draw_y++) {
        uint24_t posX = posX0;
        for (uint8_t draw_x = 0; draw_x < tilemap->draw_width; draw_x++) {
            gfx_TransparentSprite_NoClip(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

/** @todo Validate that this is correct for graphx */
uint8_t *gfx_TilePtr(const gfx_tilemap_t *tilemap, uint24_t x_offset, uint24_t y_offset) {
    return lib.gfz_TilePtrMapped(reinterpret_cast<const gfz_tilemap_t*>(tilemap), x_offset, y_offset);
}

/** @todo Validate that this is correct for graphx */
uint8_t *gfx_TilePtrMapped(const gfx_tilemap_t *tilemap, uint8_t col, uint8_t row) {
    return lib.gfz_TilePtrMapped(reinterpret_cast<const gfz_tilemap_t*>(tilemap), col, row);
}

//------------------------------------------------------------------------------
// RLET Sprites
//------------------------------------------------------------------------------

/* gfx_RLETSprite */



/* gfx_RLETSprite_NoClip */



/* gfx_ConvertFromRLETSprite */



/* gfx_ConvertToRLETSprite */



/* gfx_ConvertToNewRLETSprite */



//------------------------------------------------------------------------------

#endif
