#ifndef GRAPHZ_HPP
#define GRAPHZ_HPP

#include <cstdint>
#include <cstddef>
#include <cstdarg>
#include <cstring>
#include <PortCE.h>
#include <sys/lcd.h>
#include <cstdio>
#include <algorithm>

#include "graphz_palette.h"

//------------------------------------------------------------------------------
// Common types and macros
//------------------------------------------------------------------------------

typedef struct gfz_region_t {
    int xmin;
    int ymin;
    int xmax;
    int ymax;
} gfz_region_t;

typedef struct gfz_sprite_t {
    uint8_t width;
    uint8_t height;
    uint8_t data[];
} gfz_sprite_t;

typedef struct gfz_rletsprite_t {
    uint8_t width;
    uint8_t height;
    uint8_t data[];
} gfz_rletsprite_t;

typedef enum {
    gfz_tile_no_pow2 = 0,
    gfz_tile_2_pixel,
    gfz_tile_4_pixel,
    gfz_tile_8_pixel,
    gfz_tile_16_pixel,
    gfz_tile_32_pixel,
    gfz_tile_64_pixel,
    gfz_tile_128_pixel
} gfz_tilemap_type_t;

typedef struct gfz_tilemap_t {
    uint8_t *map;
    gfz_sprite_t **tiles;
    uint8_t tile_height;
    uint8_t tile_width;
    uint8_t draw_height;
    uint8_t draw_width;
    uint8_t type_width;
    uint8_t type_height;
    uint8_t height;
    uint8_t width;
    uint8_t y_loc;
    uint24_t x_loc;
} gfz_tilemap_t;

typedef enum {
    gfz_screen = 0,
    gfz_buffer
} gfz_location_t;

typedef enum {
    gfz_text_clip = 1,
    gfz_text_noclip
} gfz_text_options_t;

#define gfz_palette \
((uint16_t*)0xE30200)

#define gfz_vram \
((uint8_t*)0xD40000)

#define GFZ_LCD_WIDTH \
(320)

#define GFZ_LCD_HEIGHT \
(240)

#define gfz_vbuffer \
(**(uint8_t(**)[240][320])0xE30014)

#define gfz_SetDrawBuffer() \
gfz_SetDraw(gfz_buffer)

#define gfz_SetDrawScreen() \
gfz_SetDraw(gfz_screen)

#define gfz_BlitScreen() \
gfz_Blit(gfz_screen)

#define gfz_BlitBuffer() \
gfz_Blit(gfz_buffer)

//------------------------------------------------------------------------------
// GraphZ
//------------------------------------------------------------------------------

#define CurrentBuffer lcd_LpBase


struct GraphZ {
public:
const char * library_name;
GraphZ(const char* name) : library_name(name) {}
static constexpr uint8_t Maximum_Font_Width = 8;
static constexpr uint8_t Maximum_Font_Height = 8;
int32_t ClipXMin;
int32_t ClipXMax;
int32_t ClipYMin;
int32_t ClipYMax;
uint8_t Color;
uint8_t Transparent_Color;
uint8_t Text_FG_Color;
uint8_t Text_BG_Color;
uint8_t Text_TP_Color;
const uint8_t *CharSpacing;
const uint8_t *TextData;
int32_t TextXPos;
int32_t TextYPos;
uint8_t TextWidthScale;
uint8_t TextHeightScale;
uint8_t PrintChar_Clip;
uint8_t FontHeight;
uint8_t MonospaceFont = 0;

uint8_t TmpCharSprite[sizeof(gfz_sprite_t) + (Maximum_Font_Width * Maximum_Font_Height)] = {
    Maximum_Font_Width, Maximum_Font_Height
};

static constexpr int8_t SinTable[65] = {
    0  ,3  ,6  ,9  ,  13 ,16 ,19 ,22 ,  25 ,28 ,31 ,34 ,  37 ,40 ,43 ,46 ,
    49 ,52 ,55 ,58 ,  60 ,63 ,66 ,68 ,  71 ,74 ,76 ,79 ,  81 ,84 ,86 ,88 ,
    91 ,93 ,95 ,97 ,  99 ,101,103,105,  106,108,110,111,  113,114,116,117,
    118,119,121,122,  122,123,124,125,  126,126,127,127,  127,127,127,127,
    127
};

__attribute__((__format__(__printf__, 2, 3)))
void print_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "%s", library_name);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void test_state(void) {
    if (CharSpacing == NULL) {
        print_warning("CharSpacing is NULL");
    }
    if (TextData == NULL) {
        print_warning("TextData is NULL");
    }
    if (TextWidthScale == 0) {
        print_warning("TextWidthScale is zero");
    }
    if (TextHeightScale == 0) {
        print_warning("TextHeightScale is zero");
    }
    if (FontHeight > Maximum_Font_Height) {
        print_warning("FontHeight(%d) > Maximum_Font_Height", FontHeight);
    }
    if (
        (ClipXMin >= ClipXMax || ClipYMin >= ClipYMax) ||
        (ClipXMax > GFZ_LCD_WIDTH || ClipYMax > GFZ_LCD_HEIGHT)
    ) {
        print_warning(
            "invalid clipping bounds: min(%d, %d) max(%d, %d)",
            (int)ClipXMin, (int)ClipYMin, (int)ClipXMax, (int)ClipYMax
        );
    }
    if (
        (TextXPos < 0 || TextXPos >= GFZ_LCD_WIDTH) ||
        (TextYPos < 0 || TextYPos >= GFZ_LCD_HEIGHT)
    ) {
        print_warning(
            "text position out of bounds: (%d, %d)",
            (int)TextXPos, (int)TextYPos
        );
    }
}

//------------------------------------------------------------------------------
// Colors and Palette
//------------------------------------------------------------------------------

uint8_t gfz_SetColor(uint8_t index) {
    uint8_t prev = Color;
    Color = index;
    return prev;
}

uint8_t gfz_SetTransparentColor(uint8_t index) {
    uint8_t prev = Transparent_Color;
    Transparent_Color = index;
    return prev;
}

uint8_t gfz_SetTextFGColor(uint8_t color) {
    uint8_t prev = Color;
    Color = color;
    return prev;
}

uint8_t gfz_SetTextBGColor(uint8_t color) {
    uint8_t prev = Color;
    Color = color;
    return prev;
}

uint8_t gfz_SetTextTransparentColor(uint8_t color) {
    uint8_t prev = Color;
    Color = color;
    return prev;
}

void gfz_SetPalette(
    const void *palette,
    uint24_t size,
    uint8_t offset
) {
    if ((2 * offset) + size > 512) {
        size = 512 - (2 * offset);
    }
    memcpy(&lcd_Palette[offset], palette, size);
}

void gfz_SetDefaultPalette(void) {
    memcpy(lcd_Palette, default_palette_8bpp, sizeof(default_palette_8bpp));
}

uint16_t gfz_Darken(uint16_t color, uint8_t amount) {
    uint8_t r = (uint8_t)(color & 0x1F);
    uint8_t g = (uint8_t)((color & 0x3E0) >> 4) + ((color & 0x8000) ? 1 : 0);
    uint8_t b = (uint8_t)((color & 0x7C00) >> 10);
    r = (r * amount + 128) / 256;
    g = (g * amount + 128) / 256;
    b = (b * amount + 128) / 256;
    return ((g & 0x1) ? 0x8000 : 0x0000) | (r << 10) | ((g >> 1) << 5) | b;
}

uint16_t gfz_Lighten(uint16_t color, uint8_t amount) {
    return ~gfz_Darken(~color, amount);
}

//------------------------------------------------------------------------------
// Rendering Primitives
//------------------------------------------------------------------------------

void gfz_Begin(void) {
    // ti.boot.ClearVRAM
    memset(gfz_vram, 0xFF, GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT * 2);
    gfz_SetDefaultPalette();
    gfz_SetDraw(gfz_screen);
    lcd_UpBase = RAM_OFFSET(gfz_vram);
    lcd_LpBase = RAM_OFFSET(gfz_vram);

    // Resetting temp globals
    Color = 0;
    Transparent_Color = 0;

    Text_FG_Color = 0;
    Text_BG_Color = 255;
    Text_TP_Color = 255;

    TextXPos = 0;
    TextYPos = 0;

    TextWidthScale = 1;
    TextHeightScale = 1;

    PrintChar_Clip = gfz_text_noclip;
    FontHeight = Maximum_Font_Height;

    ClipXMin = 0;
    ClipYMin = 0;
    ClipXMax = GFZ_LCD_WIDTH;
    ClipYMax = GFZ_LCD_HEIGHT;

    MonospaceFont = 0;
}

void gfz_End(void) {
    lcd_UpBase = RAM_OFFSET(gfz_vram);
    memset(gfz_vram, 0xFF, sizeof(uint16_t) * GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
}

void gfz_Wait(void) {
    PortCE_new_frame();
}

void gfz_SwapDraw(void) {
    const uint24_t temp = lcd_UpBase;
    lcd_UpBase = CurrentBuffer;
    CurrentBuffer = temp;
    gfz_Wait();
}

uint8_t gfz_GetDraw(void) {
    // This is what the assembly does
    // (0xD40000 >> 16) ^ (0xD52C00 >> 16) == 0xD4 ^ 0xD5
    return ((CurrentBuffer >> 16) ^ (RAM_OFFSET(gfz_vram) >> 16)) ? 1 : 0;
}

void gfz_SetDraw(uint8_t location) {
    switch (location) {
        case gfz_screen:
            CurrentBuffer = lcd_UpBase;
            return;
        default:
        case gfz_buffer:
            if (lcd_UpBase == RAM_OFFSET(gfz_vram)) {
                CurrentBuffer = RAM_OFFSET(gfz_vram) + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
            } else {
                CurrentBuffer = RAM_OFFSET(gfz_vram);
            }
            return;
    }
}

//------------------------------------------------------------------------------
// Clipping
//------------------------------------------------------------------------------

bool util_ClipRegion(gfz_region_t * region) {
    region->xmin = std::max<int>(ClipXMin, region->xmin);
    region->xmax = std::min<int>(ClipXMax, region->xmax);
    if (region->xmax <= region->xmin) {
        return true;
    }
    region->ymin = std::max<int>(ClipYMin, region->ymin);
    region->ymax = std::min<int>(ClipYMax, region->ymax);
    if (region->ymax <= region->ymin) {
        return true;
    }
    return false;
}

void gfz_SetClipRegion(int24_t xmin, int24_t ymin, int24_t xmax, int24_t ymax) {
    gfz_region_t region;
    region.xmin = xmin;
    region.ymin = ymin;
    region.xmax = xmax;
    region.ymax = ymax;
    if (util_ClipRegion(&region)) {
        return;
    }
    ClipXMin = region.xmin;
    ClipYMin = region.ymin;
    ClipXMax = region.xmax;
    ClipYMax = region.ymax;
}

bool gfz_GetClipRegion(gfz_region_t *region) {
    return !util_ClipRegion(region);
}

//------------------------------------------------------------------------------
// Blitting
//------------------------------------------------------------------------------

void gfz_Blit(gfz_location_t src) {
    const uint8_t *src_buf = gfz_vram;
    uint8_t *dst_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
    if (src) {
        src_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
        dst_buf = gfz_vram;
    }
    memcpy(dst_buf, src_buf, GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
    gfz_Wait();
}

virtual void gfz_BlitLines(
    gfz_location_t src,
    uint8_t y_loc,
    uint8_t num_lines
) = 0;

virtual void gfz_BlitRectangle(
    gfz_location_t src,
    uint24_t x,
    uint8_t y,
    uint24_t width,
    uint24_t height
) = 0;

virtual void gfz_CopyRectangle(
    gfz_location_t src, gfz_location_t dst,
    uint24_t src_x, uint8_t src_y,
    uint24_t dst_x, uint8_t dst_y,
    uint24_t width, uint8_t height
) = 0;

//------------------------------------------------------------------------------
// Screen Shifting
//------------------------------------------------------------------------------

virtual void gfz_ShiftDown(
    uint8_t pixels
) = 0;

virtual void gfz_ShiftUp(
    uint8_t pixels
) = 0;

virtual void gfz_ShiftLeft(
    uint24_t pixels
) = 0;

virtual void gfz_ShiftRight(
    uint24_t pixels
) = 0;

//------------------------------------------------------------------------------
// Pixel Functions
//------------------------------------------------------------------------------

virtual void gfz_SetPixel_NoClip(
    uint24_t x,
    uint8_t y,
    uint8_t color
) = 0;

virtual void gfz_SetPixel(
    uint24_t x,
    uint8_t y
) = 0;

void gfz_SetPixel_ScreenClip(uint24_t x, uint8_t y) {
    if (x < GFZ_LCD_WIDTH && y < GFZ_LCD_HEIGHT) {
       gfz_SetPixel_NoClip(x, y, Color);
    }
}

void gfz_SetPixel_RegionClip(uint24_t x, uint8_t y, uint8_t color) {
    if (
        (int24_t)(x) >= ClipXMin && (int24_t)(x) < ClipXMax &&
        (int24_t)(y) >= ClipYMin && (int24_t)(y) < ClipYMax &&
        (int24_t)(x) >= 0 && (int24_t)(y) >= 0
    ) {
        gfz_SetPixel_NoClip(x, y, color);
    }
}

virtual uint8_t gfz_GetPixel(
    uint24_t x,
    uint8_t y
) = 0;

//------------------------------------------------------------------------------
// Screen Filling
//------------------------------------------------------------------------------

void gfz_FillScreen(uint8_t index) {
    memset(RAM_ADDRESS(CurrentBuffer), index, GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
    gfz_Wait();
}

void gfz_ZeroScreen(void) {
    gfz_FillScreen(0);
}

//------------------------------------------------------------------------------
// Horizontal Lines
//------------------------------------------------------------------------------

void gfz_HorizLine_NoClip(uint24_t x, uint8_t y, uint24_t length) { //x start, y postion, x length
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint24_t)y * GFZ_LCD_WIDTH));
    memset(fill, Color, length);
}

void gfz_HorizLine(int24_t x, int24_t y, int24_t length) {
    if (y < ClipYMin || y >= ClipYMax || x >= ClipXMax) {
        return;
    }
    if (x < ClipXMin) {
        length -= ClipXMin - x;
        x = 0;
    }
    length = (x + length > ClipXMax) ? (ClipXMax - x) : length;
    if (length <= 0) {
        return;
    }
    gfz_HorizLine_NoClip(x, y, length);
}

//------------------------------------------------------------------------------
// Vertical Lines
//------------------------------------------------------------------------------

virtual void gfz_VertLine_NoClip(
    uint24_t x,
    uint8_t y,
    uint24_t length
) = 0;

void gfz_VertLine(int24_t x, int24_t y, int24_t length) {
    if (x < ClipXMin || x >= ClipXMax || y >= ClipYMax) {
        return;
    }
    if (y < ClipYMin) {
        length -= ClipYMin - y;
        y = 0;
    }
    length = (y + length > ClipYMax) ? (ClipYMax - y) : length;
    if (length <= 0) {
        return;
    }
    gfz_VertLine_NoClip(x, y, length);
}

//------------------------------------------------------------------------------
// Filled Rectangle
//------------------------------------------------------------------------------

virtual void gfz_FillRectangle_NoClip(
    uint24_t x,
    uint8_t y,
    uint24_t width,
    uint8_t height
) = 0;

void gfz_FillRectangle(int24_t x, int24_t y, int24_t width, int24_t height) {
    if (x < ClipXMin) {
        width -= ClipXMin - x;
        x = 0;
    }
    if (y < ClipYMin) {
        height -= ClipYMin - y;
        y = 0;
    }
    width = (x + width > ClipXMax) ? (ClipXMax - x) : width;
    height = (y + height > ClipYMax) ? (ClipYMax - y) : height;
    if (width <= 0 || height <= 0) {
        return;
    }
    gfz_FillRectangle_NoClip(x, y, width, height);
}

//------------------------------------------------------------------------------
// Rectangle Outline
//------------------------------------------------------------------------------

virtual void gfz_Rectangle_NoClip(uint24_t x, uint8_t y, uint24_t width, uint8_t height) {
    gfz_HorizLine_NoClip(x            , y             , width );
    gfz_HorizLine_NoClip(x            , y + height - 1, width );
    gfz_VertLine_NoClip (x            , y             , height);
    gfz_VertLine_NoClip (x + width - 1, y             , height);
}

virtual void gfz_Rectangle(int24_t x, int24_t y, int24_t width, int24_t height) {
    gfz_HorizLine(x            , y             , width );
    gfz_HorizLine(x            , y + height - 1, width );
    gfz_VertLine (x            , y             , height);
    gfz_VertLine (x + width - 1, y             , height);
}

//------------------------------------------------------------------------------
// Text and Fonts
//------------------------------------------------------------------------------

void gfz_SetMonospaceFont(uint8_t spacing) {
    MonospaceFont = spacing;
}

uint8_t gfz_SetFontHeight(uint8_t height) {
    if (height <= 0 || height > Maximum_Font_Height) {
        print_warning("font height out of range: %d", height);
    }
    uint8_t prev = FontHeight;
    FontHeight = height;
    return prev;
}

void gfz_SetTextConfig(uint8_t config) {
    PrintChar_Clip = config;
}

void gfz_SetTextXY(int24_t x, int24_t y) {
    TextXPos = x;
    TextYPos = y;
    test_state();
}

int24_t gfz_GetTextY(void) {
    return TextYPos;
}

int24_t gfz_GetTextX(void) {
    return TextXPos;
}

void gfz_SetFontSpacing(const uint8_t *data, const uint8_t *DefaultCharSpacing) {
    CharSpacing = (data == NULL) ? DefaultCharSpacing : data;
}

uint8_t *gfz_SetFontData(const uint8_t *data, const uint8_t *DefaultTextData) {
    uint8_t* temp = (uint8_t*)TextData;
    TextData = (data == NULL) ? DefaultTextData : data;
    return temp;
}

uint8_t *gfz_SetCharData(uint8_t index, const uint8_t *data) {
    return static_cast<uint8_t*>(
        memcpy(&((uint8_t*)TextData)[index * 8], data, 8 * 8)
    );
}

uint24_t gfz_GetCharWidth(char c) {
    return (MonospaceFont != 0) ? MonospaceFont : CharSpacing[(unsigned char)c];
}

void gfz_SetTextScale(uint8_t width_scale, uint8_t height_scale) {
    TextWidthScale = width_scale;
    TextHeightScale = height_scale;
    test_state();
}

//------------------------------------------------------------------------------
// Character Drawing
//------------------------------------------------------------------------------

virtual void gfz_PrintChar(
    char c
) = 0;

//------------------------------------------------------------------------------
// String Functions
//------------------------------------------------------------------------------

uint24_t gfz_GetStringWidth(const char *string) {
    uint24_t len = 0;
    while (*string != '\0') {
        len += gfz_GetCharWidth(*string);
        string++;
    }
    return len * TextWidthScale;
}

void gfz_PrintUInt(uint24_t n, uint8_t length) {
    if (length < 1 || length > 8) {
        print_warning("Print(U)Int: invalid length %u\n", length);
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
        gfz_PrintChar('0');
    }
    uint8_t digit;
    switch (digit_count) {
        case 8: // 10'000'000 <= n <= 16'777'215
            gfz_PrintChar('1');
        case 7:
            digit = ((n / 1000000) % 10) + '0';
            gfz_PrintChar(digit);
        case 6:
            digit = ((n / 100000) % 10) + '0';
            gfz_PrintChar(digit);
        case 5:
            digit = ((n / 10000) % 10) + '0';
            gfz_PrintChar(digit);
        case 4:
            digit = ((n / 1000) % 10) + '0';
            gfz_PrintChar(digit);
        case 3:
            digit = ((n / 100) % 10) + '0';
            gfz_PrintChar(digit);
        case 2:
            digit = ((n / 10) % 10) + '0';
            gfz_PrintChar(digit);
        case 1:
            digit = (n % 10) + '0';
            gfz_PrintChar(digit);
        case 0:
        return;
    }
}

void gfz_PrintInt(int24_t n, uint8_t length) {
    if (n >= 0) {
        gfz_PrintUInt(n, length);
        return;
    }
    gfz_PrintChar('-');
    if (length > 1) {
        length--;
    }
    gfz_PrintUInt(-n, length);
}

void gfz_PrintString(const char *string) {
    while (*string != '\0') {
        gfz_PrintChar(*string);
        string++;
    }
}

void gfz_PrintStringXY(const char *string, int24_t x, int24_t y) {
    gfz_SetTextXY(x, y);
    gfz_PrintString(string);
}

//------------------------------------------------------------------------------
// Unclipped Lines
//------------------------------------------------------------------------------

void gfz_internal_Line0_NoClip(int24_t x0, int24_t y0, int24_t x1, int24_t y1) {
    int24_t dX = x1 - x0;
    int24_t dY = y1 - y0;
    int24_t yI = 1;
    if (dY < 0) {
        yI = -1;
        dY = -dY;
    }
    int24_t dD = 2 * dY - dX;
    const int24_t dD_jump = 2 * (dY - dX);
    dY *= 2;
    int24_t y = y0;
    for (int24_t x = x0; x < x1; x++) {
        gfz_SetPixel_NoClip(x, y, Color);
        if (dD > 0) {
            y += yI;
            dD += dD_jump;
        } else {
            dD += dY;
        }
    }
}

void gfz_internal_Line1_NoClip(int24_t x0, int24_t y0, int24_t x1, int24_t y1) {
    int24_t dX = x1 - x0;
    int24_t dY = y1 - y0;
    int24_t xI = 1;
    if (dX < 0) {
        xI = -1;
        dX = -dX;
    }
    int24_t dD = (2 * dX) - dY;
    const int24_t dD_jump = 2 * (dX - dY);
    dX *= 2;
    int24_t x = x0;

    for (int24_t y = y0; y < y1; y++) {
        gfz_SetPixel_NoClip(x, y, Color);
        if (dD > 0) {
            x += xI;
            dD += dD_jump;
        } else {
            dD += dX;
        }
    }
}

void gfz_Line_NoClip(uint24_t x0, uint8_t y0, uint24_t x1, uint8_t y1) {

    if (abs((int24_t)y1 - (int24_t)y0) < abs((int24_t)x1 - (int24_t)x0)) {
        if (x0 > x1) {
            gfz_internal_Line0_NoClip(x1, y1, x0, y0);
        } else {
            gfz_internal_Line0_NoClip(x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            gfz_internal_Line1_NoClip(x1, y1, x0, y0);
        } else {
            gfz_internal_Line1_NoClip(x0, y0, x1, y1);
        }
    }
}

//------------------------------------------------------------------------------
// Clipped Lines
//------------------------------------------------------------------------------

void gfz_internal_Line0(int24_t x0, int24_t y0, int24_t x1, int24_t y1) {
    int24_t dX = x1 - x0;
    int24_t dY = y1 - y0;
    int24_t yI = 1;
    if (dY < 0) {
        yI = -1;
        dY = -dY;
    }
    int24_t dD = 2 * dY - dX;
    const int24_t dD_jump = 2 * (dY - dX);
    dY *= 2;
    int24_t y = y0;
    for (int24_t x = x0; x < x1; x++) {
        gfz_SetPixel_RegionClip(x, y, Color);
        if (dD > 0) {
            y += yI;
            dD += dD_jump;
        } else {
            dD += dY;
        }
    }
}

void gfz_internal_Line1(int24_t x0, int24_t y0, int24_t x1, int24_t y1) {
    int24_t dX = x1 - x0;
    int24_t dY = y1 - y0;
    int24_t xI = 1;
    if (dX < 0) {
        xI = -1;
        dX = -dX;
    }
    int24_t dD = (2 * dX) - dY;
    const int24_t dD_jump = 2 * (dX - dY);
    dX *= 2;
    int24_t x = x0;

    for (int24_t y = y0; y < y1; y++) {
        gfz_SetPixel_RegionClip(x, y, Color);
        if (dD > 0) {
            x += xI;
            dD += dD_jump;
        } else {
            dD += dX;
        }
    }
}

void gfz_Line(int24_t x0, int24_t y0, int24_t x1, int24_t y1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            gfz_internal_Line0(x1, y1, x0, y0);
        } else {
            gfz_internal_Line0(x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            gfz_internal_Line1(x1, y1, x0, y0);
        } else {
            gfz_internal_Line1(x0, y0, x1, y1);
        }
    }
}

//------------------------------------------------------------------------------
// Circles
//------------------------------------------------------------------------------

// https://zingl.github.io/bresenham.html
/** @todo make function pixel perfect */
void gfz_Circle(
    const int24_t x, const int24_t y, const uint24_t radius
) {
    int24_t r = radius;

    int24_t x_pos = -r;
    int24_t y_pos = 0;
    int24_t err = 2 - 2 * r;
    do {
        gfz_SetPixel_RegionClip(x - x_pos, y + y_pos, Color);
        gfz_SetPixel_RegionClip(x - y_pos, y - x_pos, Color);
        gfz_SetPixel_RegionClip(x + x_pos, y - y_pos, Color);
        gfz_SetPixel_RegionClip(x + y_pos, y + x_pos, Color);
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
void gfz_FillCircle(
    const int24_t x, const int24_t y, const uint24_t radius
) {
    int24_t r = radius;

    int24_t x_pos = -r;
    int24_t y_pos = 0;
    int24_t err = 2 - 2 * r;
    do {
        gfz_HorizLine(x + x_pos, y - y_pos, -x_pos * 2 + 1);
        gfz_HorizLine(x + x_pos, y + y_pos, -x_pos * 2 + 1);
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
void gfz_FillCircle_NoClip(
    const uint24_t x, const uint8_t y, const uint24_t radius
) {
    int24_t r = radius;

    int24_t x_pos = -r;
    int24_t y_pos = 0;
    int24_t err = 2 - 2 * r;
    do {
        gfz_HorizLine_NoClip(x + x_pos, (uint8_t)(y - y_pos), -x_pos * 2 + 1);
        gfz_HorizLine_NoClip(x + x_pos, (uint8_t)(y + y_pos), -x_pos * 2 + 1);
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

void gfz_internal_Ellipse_dual_point(
    int24_t x, int24_t y, int24_t xc, int24_t yc
) {
    gfz_SetPixel_RegionClip(x - xc, y - yc, Color);
    gfz_SetPixel_RegionClip(x + xc, y - yc, Color);
    gfz_SetPixel_RegionClip(x - xc, y + yc, Color);
    gfz_SetPixel_RegionClip(x + xc, y + yc, Color);
}

void gfz_internal_Ellipse_dual_point_NoClip(
    int24_t x, int24_t y, int24_t xc, int24_t yc
) {
    gfz_SetPixel_NoClip(x - xc, y - yc, Color);
    gfz_SetPixel_NoClip(x + xc, y - yc, Color);
    gfz_SetPixel_NoClip(x - xc, y + yc, Color);
    gfz_SetPixel_NoClip(x + xc, y + yc, Color);
}

void gfz_internal_Ellipse_dual_line(
    int24_t x, int24_t y, int24_t xc, int24_t yc
) {
    gfz_HorizLine(x - xc, y - yc, 2 * xc);
    gfz_HorizLine(x - xc, y + yc, 2 * xc);
}

void gfz_internal_Ellipse_dual_line_NoClip(
    int24_t x, int24_t y, int24_t xc, int24_t yc
) {
    gfz_HorizLine_NoClip(x - xc, (uint8_t)(y - yc), 2 * xc);
    gfz_HorizLine_NoClip(x - xc, (uint8_t)(y + yc), 2 * xc);
}

// Derived from graphx.asm
void gfz_internal_Ellipse(
    int24_t x, int24_t y, uint24_t a, uint24_t b,
    void (GraphZ::*plot_function)(int24_t, int24_t, int24_t, int24_t)
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
        (this->*plot_function)(x, y, xc, yc);
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
        (this->*plot_function)(x, y, xc, yc);
        if (sigma >= 0) {
            sigma += sigma_add_2;
            sigma_add_2 += fb2;
            xc--;
        }

        sigma += a2 * (4 * yc + 6);
        yc++;
    }
}

virtual void gfz_Ellipse(int24_t x, int24_t y, uint24_t a, uint24_t b) {
    gfz_internal_Ellipse(
        x, y, a, b,
        &GraphZ::gfz_internal_Ellipse_dual_point
    );
}

virtual void gfz_Ellipse_NoClip(uint24_t x, uint24_t y, uint8_t a, uint8_t b) {
    gfz_internal_Ellipse(
        (int24_t)x, (int24_t)y, (uint24_t)a, (uint24_t)b,
        &GraphZ::gfz_internal_Ellipse_dual_point_NoClip
    );
}

virtual void gfz_FillEllipse(int24_t x, int24_t y, uint24_t a, uint24_t b) {
    gfz_internal_Ellipse(
        x, y, a, b,
        &GraphZ::gfz_internal_Ellipse_dual_line
    );
}

virtual void gfz_FillEllipse_NoClip(uint24_t x, uint24_t y, uint8_t a, uint8_t b) {
    gfz_internal_Ellipse(
        (int24_t)x, (int24_t)y, (uint24_t)a, (uint24_t)b,
        &GraphZ::gfz_internal_Ellipse_dual_line_NoClip
    );
}

//------------------------------------------------------------------------------
// Triangles
//------------------------------------------------------------------------------

// y2 >= y1 >= y0
void gfz_internal_triangle_sort(
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

void gfz_FillTriangle(
    int24_t x0, int24_t y0,
    int24_t x1, int24_t y1,
    int24_t x2, int24_t y2
) {
    gfz_internal_triangle_sort(
        &x0, &y0,
        &x1, &y1,
        &x2, &y2
    );
    if (y0 == y2) { // Degenerate Triangle
        int24_t x_min = (x0 < x1) ? x0 : x1;
        x_min = (x_min < x2) ? x_min : x2;
        int24_t x_max = (x0 > x1) ? x0 : x1;
        x_max = (x_max > x2) ? x_max : x2;
        gfz_HorizLine(x_min, y0, x_max - x_min + 1);
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
        gfz_HorizLine(a, y, b - a + 1);
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
        gfz_HorizLine(a, y, b - a + 1);
    }
}

void gfz_FillTriangle_NoClip(
    int24_t x0, int24_t y0,
    int24_t x1, int24_t y1,
    int24_t x2, int24_t y2
) {
    gfz_internal_triangle_sort(
        &x0, &y0,
        &x1, &y1,
        &x2, &y2
    );
    if (y0 == y2) { // Degenerate Triangle
        int24_t x_min = (x0 < x1) ? x0 : x1;
        x_min = (x_min < x2) ? x_min : x2;
        int24_t x_max = (x0 > x1) ? x0 : x1;
        x_max = (x_max > x2) ? x_max : x2;
        gfz_HorizLine_NoClip(x_min, (uint8_t)y0, x_max - x_min + 1);
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
        gfz_HorizLine_NoClip(a, (uint8_t)y, b - a + 1);
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
        gfz_HorizLine_NoClip(a, (uint8_t)y, b - a + 1);
    }
}

//------------------------------------------------------------------------------
// Unfilled Polygons
//------------------------------------------------------------------------------

void gfz_Polygon(const int24_t *points, size_t num_points) {
    if (num_points < 2) {
        return;
    }
    gfz_Line(
        points[num_points * 2 - 2], points[num_points * 2 - 1],
        points[0], points[1]
    );
    for (size_t i = 2; i < 2 * num_points; i += 2) {
        gfz_Line(
            points[i + 0], points[i + 1],
            points[i + 2], points[i + 3]
        );
    }
}

void gfz_Polygon_NoClip(const int24_t *points, size_t num_points) {
    if (num_points < 2) {
        return;
    }
    gfz_Line_NoClip(
        points[num_points * 2 - 2], points[num_points * 2 - 1],
        points[0], points[1]
    );
    for (size_t i = 2; i < 2 * num_points; i += 2) {
        gfz_Line_NoClip(
            points[i + 0], points[i + 1],
            points[i + 2], points[i + 3]
        );
    }
}

//------------------------------------------------------------------------------
// Floodfill
//------------------------------------------------------------------------------

virtual void gfz_FloodFill(
    uint24_t x,
    uint8_t y,
    uint8_t color
) = 0;

//------------------------------------------------------------------------------
// Sprite Utilities
//------------------------------------------------------------------------------

gfz_sprite_t *gfz_AllocSprite(
    uint8_t width,
    uint8_t height,
    void *(*malloc_routine)(size_t)
) {
    if (width == 0 || height == 0) {
        print_warning("sprites with zero width/height are undefined");
    }
    size_t allocation_size = sizeof(gfz_sprite_t) + (width * height);
    gfz_sprite_t* ret = (gfz_sprite_t*)((*malloc_routine)(allocation_size));
    if (ret) {
        ret->width = width;
        ret->height = height;
    }
    return ret;
}

virtual gfz_sprite_t *gfz_GetSprite(
    gfz_sprite_t *sprite_buffer,
    int24_t x,
    int24_t y
) = 0;

virtual gfz_sprite_t *gfz_GetSpriteChar(
    char c
) = 0;

//------------------------------------------------------------------------------
// Sprite Othogonal Transformations
//------------------------------------------------------------------------------

virtual gfz_sprite_t *gfz_FlipSpriteY(
    const gfz_sprite_t *sprite_in,
    gfz_sprite_t *sprite_out
) = 0;

virtual gfz_sprite_t *gfz_FlipSpriteX(
    const gfz_sprite_t *sprite_in,
    gfz_sprite_t *sprite_out
) = 0;

virtual gfz_sprite_t *gfz_RotateSpriteC(
    const gfz_sprite_t *sprite_in,
    gfz_sprite_t *sprite_out
) = 0;

virtual gfz_sprite_t *gfz_RotateSpriteCC(
    const gfz_sprite_t *sprite_in,
    gfz_sprite_t *sprite_out
) = 0;

virtual gfz_sprite_t *gfz_RotateSpriteHalf(
    const gfz_sprite_t *sprite_in,
    gfz_sprite_t *sprite_out
) = 0;

//------------------------------------------------------------------------------
// Sprite Drawing
//------------------------------------------------------------------------------

virtual void gfz_Sprite(
    const gfz_sprite_t *sprite,
    int24_t x,
    int24_t y
) = 0;

virtual void gfz_TransparentSprite(
    const gfz_sprite_t *sprite,
    int24_t x,
    int24_t y
) = 0;

virtual void gfz_Sprite_NoClip(
    const gfz_sprite_t *sprite,
    uint24_t x,
    uint8_t y
) = 0;

virtual void gfz_TransparentSprite_NoClip(
    const gfz_sprite_t *sprite,
    uint24_t x,
    uint8_t y
) = 0;

//------------------------------------------------------------------------------
// Scaled Sprites
//------------------------------------------------------------------------------

virtual void gfz_ScaledSprite_NoClip(
    const gfz_sprite_t *sprite,
    const uint24_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) = 0;

virtual void gfz_ScaledTransparentSprite_NoClip(
    const gfz_sprite_t *sprite,
    const uint24_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) = 0;

virtual gfz_sprite_t* gfz_ScaleSprite(
    const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out
) = 0;

//------------------------------------------------------------------------------
// Sprite Rotation
//------------------------------------------------------------------------------

virtual gfz_sprite_t *gfz_RotateScaleSprite(
    const gfz_sprite_t *sprite_in,
    gfz_sprite_t *sprite_out,
    uint8_t angle,
    uint8_t scale
) = 0;

virtual uint8_t gfz_RotatedScaledTransparentSprite(
    const gfz_sprite_t *sprite,
    uint24_t x,
    uint8_t y,
    uint8_t angle,
    uint8_t scale
) = 0;

virtual uint8_t gfz_RotatedScaledTransparentSprite_NoClip(
    const gfz_sprite_t *sprite,
    uint24_t x,
    uint8_t y,
    uint8_t angle,
    uint8_t scale
) = 0;

virtual uint8_t gfz_RotatedScaledSprite(
    const gfz_sprite_t *sprite,
    uint24_t x,
    uint8_t y,
    uint8_t angle,
    uint8_t scale
) = 0;

virtual uint8_t gfz_RotatedScaledSprite_NoClip(
    const gfz_sprite_t *sprite,
    uint24_t x,
    uint8_t y,
    uint8_t angle,
    uint8_t scale
) = 0;

//------------------------------------------------------------------------------
// Tilemaps
//------------------------------------------------------------------------------

void gfz_Tilemap(const gfz_tilemap_t* tilemap, uint24_t x_offset, uint24_t y_offset) {
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
            gfz_Sprite(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

void gfz_Tilemap_NoClip(const gfz_tilemap_t *tilemap, uint24_t x_offset, uint24_t y_offset) {
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
            gfz_Sprite_NoClip(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

void gfz_TransparentTilemap(const gfz_tilemap_t* tilemap, uint24_t x_offset, uint24_t y_offset) {
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
            gfz_TransparentSprite(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

void gfz_TransparentTilemap_NoClip(const gfz_tilemap_t *tilemap, uint24_t x_offset, uint24_t y_offset) {
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
            gfz_TransparentSprite_NoClip(tilemap->tiles[tilemap->map[map_index]], posX, posY);
            posX += tilemap->tile_width;
            map_index++;
        }
        map_index += map_jump;
        posY += tilemap->tile_height;
    }
}

/** @todo Validate that this is correct for graphx */
uint8_t *gfz_TilePtr(const gfz_tilemap_t *tilemap, uint24_t x_offset, uint24_t y_offset) {
    uint24_t map_row = x_offset / tilemap->tile_width;
    uint24_t map_col = y_offset / tilemap->tile_height;

    uint24_t map_index = map_row + (map_col * tilemap->width);
    return &(tilemap->map[map_index]);
}

/** @todo Validate that this is correct for graphx */
uint8_t *gfz_TilePtrMapped(const gfz_tilemap_t *tilemap, uint8_t col, uint8_t row) {
    uint24_t map_index = row + (col * tilemap->width);
    return &(tilemap->map[map_index]);
}

//------------------------------------------------------------------------------
// RLET Sprites
//------------------------------------------------------------------------------

virtual void gfz_RLETSprite(
    const gfz_rletsprite_t *sprite,
    int x,
    int y
) = 0;

virtual void gfz_RLETSprite_NoClip(
    const gfz_rletsprite_t *sprite,
    uint24_t x,
    uint8_t y
) = 0;

virtual gfz_sprite_t *gfz_ConvertFromRLETSprite(
    const gfz_rletsprite_t *sprite_in,
    gfz_sprite_t *sprite_out
) = 0;

virtual gfz_rletsprite_t *gfz_ConvertToRLETSprite(
    const gfz_sprite_t *sprite_in,
    gfz_rletsprite_t *sprite_out
) = 0;

virtual gfz_rletsprite_t *gfz_ConvertToNewRLETSprite(
    const gfz_sprite_t *sprite_in,
    void *(*malloc_routine)(size_t)
) = 0;

//------------------------------------------------------------------------------

};

#endif /* GRAPHZ_HPP */
