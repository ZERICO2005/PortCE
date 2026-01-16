#ifndef GRAPHZ_HPP
#define GRAPHZ_HPP

#include "../PortCE_assert.h"

#include <cstdint>
#include <cstddef>
#include <cstdarg>
#include <cstring>
#include <PortCE.h>
#include <sys/lcd.h>
#include <cstdio>
#include <algorithm>

#include "graphz_palette.h"

#define lcd_Control              (*(volatile uint24_t*)RAM_ADDRESS(0xE30018))
#define lcd_VideoMode            (*(volatile uint16_t*)RAM_ADDRESS(0xE30018))
#define lcd_UpBase               (*(volatile uint24_t*)RAM_ADDRESS(0xE30010))
#define lcd_LpBase               (*(volatile uint24_t*)RAM_ADDRESS(0xE30014))
#define lcd_BGR8bit 0x927
#define lcd_BGR16bit 0x92D

//------------------------------------------------------------------------------
// Common types and macros
//------------------------------------------------------------------------------

typedef struct gfz_region_t {
    ti_int xmin;
    ti_int ymin;
    ti_int xmax;
    ti_int ymax;
} gfz_region_t;

typedef struct __attribute__((__packed__)) gfz_sprite_t {
    uint8_t width;
    uint8_t height;
    uint8_t data[];
} gfz_sprite_t;

typedef struct __attribute__((__packed__)) gfz_rletsprite_t {
    uint8_t width;
    uint8_t height;
    uint8_t data[];
} gfz_rletsprite_t;

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
    gfz_tile_no_pow2 = 0,
    gfz_tile_2_pixel,
    gfz_tile_4_pixel,
    gfz_tile_8_pixel,
    gfz_tile_16_pixel,
    gfz_tile_32_pixel,
    gfz_tile_64_pixel,
    gfz_tile_128_pixel
} gfz_tilemap_type_t;

typedef enum {
    gfz_8bpp = 0x27 /**< Enable 8 bits per pixel mode. */
} gfz_mode_t;

typedef enum {
    gfz_screen = 0, /**< Visible Screen. */
    gfz_buffer      /**< Non-visible Buffer. */
} gfz_location_t;

typedef enum {
    gfz_text_clip = 1, /**< Text routines will clip against the defined clip window. */
    gfz_text_noclip    /**< Default, text routines do not clip (much faster). */
} gfz_text_options_t;

#define gfz_palette \
((uint16_t*)RAM_ADDRESS(0xE30200))

#define gfz_vram \
((uint8_t*)RAM_ADDRESS(0xD40000))

#define GFZ_LCD_WIDTH \
(320)

#define GFZ_LCD_HEIGHT \
(240)

#define gfz_vbuffer \
(**(uint8_t(**)[240][320])RAM_ADDRESS(0xE30014))

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

template <typename T>
struct GraphZ {
public:
const char * library_name;

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

const uint8_t *DefaultCharSpacing;
const uint8_t *DefaultTextData;

GraphZ(
    const char* name,
    const uint8_t *default_char_spacing,
    const uint8_t *default_text_data
) :
    library_name(name),
    DefaultCharSpacing(default_char_spacing),
    DefaultTextData(default_text_data)
{}

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
    fprintf(stderr, "%s: ", library_name);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void test_state(void) {
    if (CharSpacing == nullptr) {
        print_warning("CharSpacing is nullptr");
    }
    if (TextData == nullptr) {
        print_warning("TextData is nullptr");
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
        (ClipXMax > GFZ_LCD_WIDTH || ClipYMax > GFZ_LCD_HEIGHT) ||
        (ClipXMin < 0 || ClipYMin < 0) ||
        (ClipXMin >= ClipXMax || ClipYMin >= ClipYMax)
    ) {
        print_warning(
            "invalid clipping bounds: min(%d, %d) max(%d, %d)",
            (int)ClipXMin, (int)ClipYMin, (int)ClipXMax, (int)ClipYMax
        );
    }
}

//------------------------------------------------------------------------------
// External Functions
//------------------------------------------------------------------------------

void gfz_Begin(void);

void gfz_End(void);

gfz_sprite_t *gfz_AllocSprite(uint8_t width, uint8_t height, void *(*malloc_routine)(size_t));

void gfz_Tilemap(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset);

void gfz_Tilemap_NoClip(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset);

void gfz_TransparentTilemap(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset);

void gfz_TransparentTilemap_NoClip(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset);

uint8_t *gfz_TilePtr(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset);

uint8_t *gfz_TilePtrMapped(const gfz_tilemap_t *tilemap, uint8_t col, uint8_t row);

uint8_t gfz_SetColor(uint8_t index);

uint8_t gfz_SetTransparentColor(uint8_t index);

void gfz_SetDefaultPalette(gfz_mode_t mode);

void gfz_SetPalette(const void *palette, size_t size, uint8_t offset);

void gfz_FillScreen(uint8_t index);

void gfz_ZeroScreen(void);

void gfz_SetPixel(uint32_t x, uint8_t y);

uint8_t gfz_GetPixel(uint32_t x, uint8_t y);

void gfz_Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1);

void gfz_Line_NoClip(uint32_t x0, uint8_t y0, uint32_t x1, uint8_t y1);

void gfz_HorizLine(int32_t x, int32_t y, int32_t length);

void gfz_HorizLine_NoClip(uint32_t x, uint8_t y, uint32_t length);

void gfz_VertLine(int32_t x, int32_t y, int32_t length);

void gfz_VertLine_NoClip(uint32_t x, uint8_t y, uint32_t length);

void gfz_Rectangle(int32_t x, int32_t y, int32_t width, int32_t height);

void gfz_Rectangle_NoClip(uint32_t x, uint8_t y, uint32_t width, uint8_t height);

void gfz_FillRectangle(int32_t x, int32_t y, int32_t width, int32_t height);

void gfz_FillRectangle_NoClip(uint32_t x, uint8_t y, uint32_t width, uint8_t height);

void gfz_Circle(int32_t x, int32_t y, uint32_t radius);

void gfz_FillCircle(int32_t x, int32_t y, uint32_t radius);

void gfz_FillCircle_NoClip(uint32_t x, uint8_t y, uint32_t radius);

void gfz_FillEllipse_NoClip(uint32_t x, uint32_t y, uint8_t a, uint8_t b);

void gfz_FillEllipse(int32_t x, int32_t y, uint32_t a, uint32_t b);

void gfz_Ellipse_NoClip(uint32_t x, uint32_t y, uint8_t a, uint8_t b);

void gfz_Ellipse(int32_t x, int32_t y, uint32_t a, uint32_t b);

void gfz_Polygon(const ti_int *points, size_t num_points);

void gfz_Polygon_NoClip(const ti_int *points, size_t num_points);

void gfz_FillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

void gfz_FillTriangle_NoClip(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

void gfz_SetDraw(uint8_t location);

uint8_t gfz_GetDraw(void);

void gfz_SwapDraw(void);

void gfz_Wait(void);

void gfz_Blit(gfz_location_t src);

void gfz_BlitRows(gfz_location_t src, uint8_t y_loc, uint8_t num_rows);

void gfz_BlitColumns(gfz_location_t src, uint32_t x_loc, uint32_t num_columns);

void gfz_BlitRectangle(gfz_location_t src, uint32_t x, uint8_t y, uint32_t width, uint32_t height);

void gfz_CopyRectangle(gfz_location_t src, gfz_location_t dst, uint32_t src_x, uint8_t src_y, uint32_t dst_x, uint8_t dst_y, uint32_t width, uint8_t height);

void gfz_SetTextScale(uint8_t width_scale, uint8_t height_scale);

void gfz_PrintChar(const char c);

void gfz_PrintInt(int24_t n, uint8_t length);

void gfz_PrintUInt(uint24_t n, uint8_t length);

void gfz_PrintString(const char *string);

void gfz_PrintStringXY(const char *string, int32_t x, int32_t y);

int32_t gfz_GetTextX(void);

int32_t gfz_GetTextY(void);

void gfz_SetTextXY(int32_t x, int32_t y);

void gfz_SetTextConfig(uint8_t config);

uint8_t gfz_SetTextFGColor(uint8_t color);

uint8_t gfz_SetTextBGColor(uint8_t color);

uint8_t gfz_SetTextTransparentColor(uint8_t color);

void gfz_Sprite(const gfz_sprite_t *sprite, int32_t x, int32_t y);

void gfz_Sprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y);

void gfz_TransparentSprite(const gfz_sprite_t *sprite, int32_t x, int32_t y);

void gfz_TransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y);

gfz_sprite_t *gfz_GetSprite(gfz_sprite_t *sprite_buffer, int32_t x, int32_t y);

void gfz_ScaledSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y, uint8_t width_scale, uint8_t height_scale);

void gfz_ScaledTransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y, uint8_t width_scale, uint8_t height_scale);

uint8_t gfz_RotatedScaledTransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y, uint8_t angle, uint8_t scale);

uint8_t gfz_RotatedScaledSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y, uint8_t angle, uint8_t scale);

uint8_t gfz_RotatedScaledTransparentSprite(const gfz_sprite_t *sprite, int32_t x, int32_t y, uint8_t angle, uint8_t scale);

uint8_t gfz_RotatedScaledSprite(const gfz_sprite_t *sprite, int32_t x, int32_t y, uint8_t angle, uint8_t scale);

gfz_sprite_t *gfz_FlipSpriteX(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out);

gfz_sprite_t *gfz_FlipSpriteY(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out);

gfz_sprite_t *gfz_RotateSpriteC(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out);

gfz_sprite_t *gfz_RotateSpriteCC(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out);

gfz_sprite_t *gfz_RotateSpriteHalf(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out);

gfz_sprite_t *gfz_ScaleSprite(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out);

gfz_sprite_t *gfz_RotateScaleSprite(const gfz_sprite_t *__restrict sprite_in, gfz_sprite_t *__restrict sprite_out, uint8_t angle, uint8_t scale);

gfz_sprite_t *gfz_GetSpriteChar(char c);

uint8_t *gfz_SetFontData(const uint8_t *data);

uint8_t *gfz_SetCharData(uint8_t index, const uint8_t *data);

void gfz_SetFontSpacing(const uint8_t *spacing);

uint8_t gfz_SetFontHeight(uint8_t height);

void gfz_SetMonospaceFont(uint8_t spacing);

uint32_t gfz_GetStringWidth(const char *string);

uint32_t gfz_GetCharWidth(const char c);

void gfz_SetClipRegion(int32_t xmin, int32_t ymin, int32_t xmax, int32_t ymax);

bool gfz_GetClipRegion(gfz_region_t *region);

void gfz_ShiftDown(uint8_t pixels);

void gfz_ShiftUp(uint8_t pixels);

void gfz_ShiftLeft(uint32_t pixels);

void gfz_ShiftRight(uint32_t pixels);

uint16_t gfz_Lighten(uint16_t color, uint8_t amount) const;

uint16_t gfz_Darken(uint16_t color, uint8_t amount) const;

void gfz_FloodFill(uint32_t x, uint8_t y, uint8_t color);

void gfz_RLETSprite(const gfz_rletsprite_t *sprite, int32_t x, int32_t y);

void gfz_RLETSprite_NoClip(const gfz_rletsprite_t *sprite, uint32_t x, uint8_t y);

gfz_sprite_t *gfz_ConvertFromRLETSprite(const gfz_rletsprite_t *sprite_in, gfz_sprite_t *sprite_out);

gfz_rletsprite_t *gfz_ConvertToRLETSprite(const gfz_sprite_t *sprite_in, gfz_rletsprite_t *sprite_out);

gfz_rletsprite_t *gfz_ConvertToNewRLETSprite(const gfz_sprite_t *sprite_in, void *(*malloc_routine)(size_t));

//------------------------------------------------------------------------------
// External Functions
//------------------------------------------------------------------------------

void gfz_SetPixel_NoClip(uint32_t x, uint8_t y, uint8_t color);

};

//------------------------------------------------------------------------------
// Colors and Palette
//------------------------------------------------------------------------------

template<typename T>
uint8_t GraphZ<T>::gfz_SetColor(uint8_t index) {
    uint8_t prev = Color;
    Color = index;
    return prev;
}

template<typename T>
uint8_t GraphZ<T>::gfz_SetTransparentColor(uint8_t index) {
    uint8_t prev = Transparent_Color;
    Transparent_Color = index;
    return prev;
}

template<typename T>
uint8_t GraphZ<T>::gfz_SetTextFGColor(uint8_t color) {
    uint8_t prev = Text_FG_Color;
    Text_FG_Color = color;
    return prev;
}

template<typename T>
uint8_t GraphZ<T>::gfz_SetTextBGColor(uint8_t color) {
    uint8_t prev = Text_BG_Color;
    Text_BG_Color = color;
    return prev;
}

template<typename T>
uint8_t GraphZ<T>::gfz_SetTextTransparentColor(uint8_t color) {
    uint8_t prev = Text_TP_Color;
    Text_TP_Color = color;
    return prev;
}

template<typename T>
void GraphZ<T>::gfz_SetPalette(
    const void *palette,
    size_t size,
    uint8_t offset
) {
    if ((2 * offset) + size > 512) {
        print_warning("SetPalette out of bounds: (2 * offset) + size > 512");
        size = 512 - (2 * offset);
    }
    memcpy(&lcd_Palette[offset], palette, size);
}

template<typename T>
void GraphZ<T>::gfz_SetDefaultPalette(__attribute__((__unused__)) gfz_mode_t mode) {
    memcpy(lcd_Palette, default_palette_8bpp, sizeof(default_palette_8bpp));
}

template<typename T>
uint16_t GraphZ<T>::gfz_Darken(uint16_t color, uint8_t amount) const {
    uint16_t internal_gfz_Darken(uint16_t color, uint8_t amount);
    return internal_gfz_Darken(color, amount);
}

template<typename T>
uint16_t GraphZ<T>::gfz_Lighten(uint16_t color, uint8_t amount) const {
    return ~gfz_Darken(~color, amount);
}

//------------------------------------------------------------------------------
// Rendering Primitives
//------------------------------------------------------------------------------

template<typename T>
void util_Begin(GraphZ<T>& lib) {
    assert_PortCE_initialized();
    // ti.boot.ClearVRAM
    memset(gfz_vram, 0xFF, GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT * 2);
    lib.gfz_SetDefaultPalette(gfz_8bpp);
    lib.gfz_SetDraw(gfz_screen);
    lcd_UpBase = RAM_OFFSET(gfz_vram);
    lcd_LpBase = RAM_OFFSET(gfz_vram);

    // Resetting temp globals
    lib.Color = 0;
    lib.Transparent_Color = 0;

    lib.Text_FG_Color = 0;
    lib.Text_BG_Color = 255;
    lib.Text_TP_Color = 255;

    lib.TextXPos = 0;
    lib.TextYPos = 0;

    lib.TextWidthScale = 1;
    lib.TextHeightScale = 1;

    lib.PrintChar_Clip = gfz_text_noclip;
    lib.FontHeight = lib.Maximum_Font_Height;

    lib.ClipXMin = 0;
    lib.ClipYMin = 0;
    lib.ClipXMax = GFZ_LCD_WIDTH;
    lib.ClipYMax = GFZ_LCD_HEIGHT;

    lib.CharSpacing = lib.DefaultCharSpacing;
    lib.TextData = lib.DefaultTextData;

    lib.MonospaceFont = 0;
}

template<typename T>
void util_End(__attribute__((__unused__)) GraphZ<T>& lib) {
    lcd_UpBase = RAM_OFFSET(gfz_vram);
    memset(gfz_vram, 0xFF, sizeof(uint16_t) * GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
}

template<typename T>
void GraphZ<T>::gfz_Wait(void) {
    PortCE_update_registers();
}

template<typename T>
void GraphZ<T>::gfz_SwapDraw(void) {
    const uint24_t temp = lcd_UpBase;
    lcd_UpBase = CurrentBuffer;
    CurrentBuffer = temp;
    gfz_Wait();
    PortCE_new_frame();
}

template<typename T>
uint8_t GraphZ<T>::gfz_GetDraw(void) {
    // This is what the assembly does
    // (0xD40000 >> 16) ^ (0xD52C00 >> 16) == 0xD4 ^ 0xD5
    return ((CurrentBuffer >> 16) ^ (RAM_OFFSET(gfz_vram) >> 16)) ? 1 : 0;
}

template<typename T>
void GraphZ<T>::gfz_SetDraw(uint8_t location) {
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

inline bool util_ClipRegion(
    gfz_region_t * region,
    int32_t clipXMin,
    int32_t clipYMin,
    int32_t clipXMax,
    int32_t clipYMax
) {
    region->xmin = std::max<int32_t>(clipXMin, region->xmin);
    region->xmax = std::min<int32_t>(clipXMax, region->xmax);
    if (region->xmax <= region->xmin) {
        return true;
    }
    region->ymin = std::max<int32_t>(clipYMin, region->ymin);
    region->ymax = std::min<int32_t>(clipYMax, region->ymax);
    if (region->ymax <= region->ymin) {
        return true;
    }
    return false;
}

template<typename T>
void GraphZ<T>::gfz_SetClipRegion(int32_t xmin, int32_t ymin, int32_t xmax, int32_t ymax) {
    gfz_region_t region;
    region.xmin = xmin;
    region.ymin = ymin;
    region.xmax = xmax;
    region.ymax = ymax;
    if (util_ClipRegion(&region, 0, 0, GFZ_LCD_WIDTH, GFZ_LCD_HEIGHT)) {
        return;
    }
    ClipXMin = region.xmin;
    ClipYMin = region.ymin;
    ClipXMax = region.xmax;
    ClipYMax = region.ymax;
}

template<typename T>
bool GraphZ<T>::gfz_GetClipRegion(gfz_region_t *region) {
    return !util_ClipRegion(
        region,
        this->ClipXMin,
        this->ClipYMin,
        this->ClipXMax,
        this->ClipYMax
    );
}

//------------------------------------------------------------------------------
// Blitting
//------------------------------------------------------------------------------

template<typename T>
void GraphZ<T>::gfz_Blit(gfz_location_t src) {
    const uint8_t *src_buf = gfz_vram;
    uint8_t *dst_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
    if (src) {
        src_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
        dst_buf = gfz_vram;
    }
    memcpy(dst_buf, src_buf, GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
    gfz_Wait();
}

//------------------------------------------------------------------------------
// Pixel Functions
//------------------------------------------------------------------------------

template<typename T>
void gfz_SetPixel_ScreenClip(GraphZ<T>& lib, int32_t x, int32_t y) {
    if (x < GFZ_LCD_WIDTH && y < GFZ_LCD_HEIGHT) {
       lib.gfz_SetPixel_NoClip(x, y, lib.Color);
    }
}

template<typename T>
void gfz_SetPixel_RegionClip(GraphZ<T>& lib, int32_t x, int32_t y, uint8_t color) {
    if (
        x >= lib.ClipXMin && x < lib.ClipXMax &&
        y >= lib.ClipYMin && y < lib.ClipYMax &&
        x >= 0 && y >= 0
    ) {
        lib.gfz_SetPixel_NoClip((uint32_t)x, (uint8_t)y, color);
    }
}

//------------------------------------------------------------------------------
// Screen Filling
//------------------------------------------------------------------------------

template<typename T>
void GraphZ<T>::gfz_FillScreen(uint8_t index) {
    memset(RAM_ADDRESS(CurrentBuffer), index, GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
    gfz_Wait();
}

template<typename T>
void GraphZ<T>::gfz_ZeroScreen(void) {
    gfz_FillScreen(0);
}

//------------------------------------------------------------------------------
// Horizontal Lines
//------------------------------------------------------------------------------

template<typename T>
void GraphZ<T>::gfz_HorizLine_NoClip(uint32_t x, uint8_t y, uint32_t length) {
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint24_t)y * GFZ_LCD_WIDTH));
    memset(fill, Color, length);
}

template<typename T>
void GraphZ<T>::gfz_HorizLine(int32_t x, int32_t y, int32_t length) {
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

template<typename T>
void GraphZ<T>::gfz_VertLine(int32_t x, int32_t y, int32_t length) {
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

template<typename T>
void GraphZ<T>::gfz_FillRectangle(int32_t x, int32_t y, int32_t width, int32_t height) {
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

template<typename T>
void GraphZ<T>::gfz_Rectangle_NoClip(uint32_t x, uint8_t y, uint32_t width, uint8_t height) {
    gfz_HorizLine_NoClip(x            , y             , width );
    gfz_HorizLine_NoClip(x            , y + height - 1, width );
    gfz_VertLine_NoClip (x            , y             , height);
    gfz_VertLine_NoClip (x + width - 1, y             , height);
}

template<typename T>
void GraphZ<T>::gfz_Rectangle(int32_t x, int32_t y, int32_t width, int32_t height) {
    gfz_HorizLine(x            , y             , width );
    gfz_HorizLine(x            , y + height - 1, width );
    gfz_VertLine (x            , y             , height);
    gfz_VertLine (x + width - 1, y             , height);
}

//------------------------------------------------------------------------------
// Text and Fonts
//------------------------------------------------------------------------------

template<typename T>
void GraphZ<T>::gfz_SetMonospaceFont(uint8_t spacing) {
    MonospaceFont = spacing;
}

template<typename T>
uint8_t GraphZ<T>::gfz_SetFontHeight(uint8_t height) {
    if (height <= 0 || height > Maximum_Font_Height) {
        print_warning("font height out of range: %d", height);
    }
    uint8_t prev = FontHeight;
    FontHeight = height;
    return prev;
}

template<typename T>
void GraphZ<T>::gfz_SetTextConfig(uint8_t config) {
    PrintChar_Clip = config;
}

template<typename T>
void GraphZ<T>::gfz_SetTextXY(int32_t x, int32_t y) {
    TextXPos = x;
    TextYPos = y;
    test_state();
}

template<typename T>
int32_t GraphZ<T>::gfz_GetTextY(void) {
    return TextYPos;
}

template<typename T>
int32_t GraphZ<T>::gfz_GetTextX(void) {
    return TextXPos;
}

template<typename T>
void GraphZ<T>::gfz_SetFontSpacing(const uint8_t *data) {
    CharSpacing = (data == nullptr) ? DefaultCharSpacing : data;
}

template<typename T>
uint8_t *GraphZ<T>::gfz_SetFontData(const uint8_t *data) {
    uint8_t* temp = (uint8_t*)TextData;
    TextData = (data == nullptr) ? DefaultTextData : data;
    return temp;
}

template<typename T>
uint8_t *GraphZ<T>::gfz_SetCharData(uint8_t index, const uint8_t *data) {
    return static_cast<uint8_t*>(
        memcpy(&((uint8_t*)TextData)[index * 8], data, 8 * 8)
    );
}

template<typename T>
uint32_t GraphZ<T>::gfz_GetCharWidth(char c) {
    return (MonospaceFont != 0) ? MonospaceFont : CharSpacing[(unsigned char)c];
}

template<typename T>
void GraphZ<T>::gfz_SetTextScale(uint8_t width_scale, uint8_t height_scale) {
    TextWidthScale = width_scale;
    TextHeightScale = height_scale;
    test_state();
}

//------------------------------------------------------------------------------
// Character Drawing
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// String Functions
//------------------------------------------------------------------------------

template<typename T>
uint32_t GraphZ<T>::gfz_GetStringWidth(const char *string) {
    uint32_t len = 0;
    while (*string != '\0') {
        len += gfz_GetCharWidth(*string);
        string++;
    }
    return len * TextWidthScale;
}

template<typename T>
void GraphZ<T>::gfz_PrintUInt(uint24_t n, uint8_t length) {
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

template<typename T>
void GraphZ<T>::gfz_PrintInt(int24_t n, uint8_t length) {
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

template<typename T>
void GraphZ<T>::gfz_PrintString(const char *string) {
    while (*string != '\0') {
        gfz_PrintChar(*string);
        string++;
    }
}

template<typename T>
void GraphZ<T>::gfz_PrintStringXY(const char *string, int32_t x, int32_t y) {
    gfz_SetTextXY(x, y);
    gfz_PrintString(string);
}

//------------------------------------------------------------------------------
// Unclipped Lines
//------------------------------------------------------------------------------

template<typename T>
void gfz_internal_Line0_NoClip(GraphZ<T>& lib, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    int32_t dX = x1 - x0;
    int32_t dY = y1 - y0;
    int32_t yI = 1;
    if (dY < 0) {
        yI = -1;
        dY = -dY;
    }
    int32_t dD = 2 * dY - dX;
    const int32_t dD_jump = 2 * (dY - dX);
    dY *= 2;
    int32_t y = y0;
    for (int32_t x = x0; x < x1; x++) {
        lib.gfz_SetPixel_NoClip(x, y, lib.Color);
        if (dD > 0) {
            y += yI;
            dD += dD_jump;
        } else {
            dD += dY;
        }
    }
}

template<typename T>
void gfz_internal_Line1_NoClip(GraphZ<T>& lib, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    int32_t dX = x1 - x0;
    int32_t dY = y1 - y0;
    int32_t xI = 1;
    if (dX < 0) {
        xI = -1;
        dX = -dX;
    }
    int32_t dD = (2 * dX) - dY;
    const int32_t dD_jump = 2 * (dX - dY);
    dX *= 2;
    int32_t x = x0;

    for (int32_t y = y0; y < y1; y++) {
        lib.gfz_SetPixel_NoClip(x, y, lib.Color);
        if (dD > 0) {
            x += xI;
            dD += dD_jump;
        } else {
            dD += dX;
        }
    }
}

template<typename T>
void GraphZ<T>::gfz_Line_NoClip(uint32_t x0, uint8_t y0, uint32_t x1, uint8_t y1) {

    if (abs((int32_t)y1 - (int32_t)y0) < abs((int32_t)x1 - (int32_t)x0)) {
        if (x0 > x1) {
            gfz_internal_Line0_NoClip(*this, x1, y1, x0, y0);
        } else {
            gfz_internal_Line0_NoClip(*this, x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            gfz_internal_Line1_NoClip(*this, x1, y1, x0, y0);
        } else {
            gfz_internal_Line1_NoClip(*this, x0, y0, x1, y1);
        }
    }
}

//------------------------------------------------------------------------------
// Clipped Lines
//------------------------------------------------------------------------------

template<typename T>
void gfz_internal_Line0(GraphZ<T>& lib, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    int32_t dX = x1 - x0;
    int32_t dY = y1 - y0;
    int32_t yI = 1;
    if (dY < 0) {
        yI = -1;
        dY = -dY;
    }
    int32_t dD = 2 * dY - dX;
    const int32_t dD_jump = 2 * (dY - dX);
    dY *= 2;
    int32_t y = y0;
    for (int32_t x = x0; x < x1; x++) {
        gfz_SetPixel_RegionClip(lib, x, y, lib.Color);
        if (dD > 0) {
            y += yI;
            dD += dD_jump;
        } else {
            dD += dY;
        }
    }
}

template<typename T>
void gfz_internal_Line1(GraphZ<T>& lib, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    int32_t dX = x1 - x0;
    int32_t dY = y1 - y0;
    int32_t xI = 1;
    if (dX < 0) {
        xI = -1;
        dX = -dX;
    }
    int32_t dD = (2 * dX) - dY;
    const int32_t dD_jump = 2 * (dX - dY);
    dX *= 2;
    int32_t x = x0;

    for (int32_t y = y0; y < y1; y++) {
        gfz_SetPixel_RegionClip(lib, x, y, lib.Color);
        if (dD > 0) {
            x += xI;
            dD += dD_jump;
        } else {
            dD += dX;
        }
    }
}

template<typename T>
void GraphZ<T>::gfz_Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            gfz_internal_Line0(*this, x1, y1, x0, y0);
        } else {
            gfz_internal_Line0(*this, x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            gfz_internal_Line1(*this, x1, y1, x0, y0);
        } else {
            gfz_internal_Line1(*this, x0, y0, x1, y1);
        }
    }
}

//------------------------------------------------------------------------------
// Circles
//------------------------------------------------------------------------------

// https://zingl.github.io/bresenham.html
/** @todo make function pixel perfect */
template<typename T>
void GraphZ<T>::gfz_Circle(
    const int32_t x, const int32_t y, const uint32_t radius
) {
    int32_t r = radius;

    int32_t x_pos = -r;
    int32_t y_pos = 0;
    int32_t err = 2 - 2 * r;
    do {
        gfz_SetPixel_RegionClip(*this, x - x_pos, y + y_pos, Color);
        gfz_SetPixel_RegionClip(*this, x - y_pos, y - x_pos, Color);
        gfz_SetPixel_RegionClip(*this, x + x_pos, y - y_pos, Color);
        gfz_SetPixel_RegionClip(*this, x + y_pos, y + x_pos, Color);
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
template<typename T>
void GraphZ<T>::gfz_FillCircle(
    const int32_t x, const int32_t y, const uint32_t radius
) {
    int32_t r = radius;

    int32_t x_pos = -r;
    int32_t y_pos = 0;
    int32_t err = 2 - 2 * r;
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
template<typename T>
void GraphZ<T>::gfz_FillCircle_NoClip(
    const uint32_t x, const uint8_t y, const uint32_t radius
) {
    int32_t r = radius;

    int32_t x_pos = -r;
    int32_t y_pos = 0;
    int32_t err = 2 - 2 * r;
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

template<typename T>
void gfz_internal_Ellipse_dual_point(
    GraphZ<T>& lib, int32_t x, int32_t y, int32_t xc, int32_t yc
) {
    gfz_SetPixel_RegionClip(lib, x - xc, y - yc, lib.Color);
    gfz_SetPixel_RegionClip(lib, x + xc, y - yc, lib.Color);
    gfz_SetPixel_RegionClip(lib, x - xc, y + yc, lib.Color);
    gfz_SetPixel_RegionClip(lib, x + xc, y + yc, lib.Color);
}

template<typename T>
void gfz_internal_Ellipse_dual_point_NoClip(
    GraphZ<T>& lib, int32_t x, int32_t y, int32_t xc, int32_t yc
) {
    lib.gfz_SetPixel_NoClip(x - xc, y - yc, lib.Color);
    lib.gfz_SetPixel_NoClip(x + xc, y - yc, lib.Color);
    lib.gfz_SetPixel_NoClip(x - xc, y + yc, lib.Color);
    lib.gfz_SetPixel_NoClip(x + xc, y + yc, lib.Color);
}

template<typename T>
void gfz_internal_Ellipse_dual_line(
    GraphZ<T>& lib, int32_t x, int32_t y, int32_t xc, int32_t yc
) {
    lib.gfz_HorizLine(x - xc, y - yc, 2 * xc);
    lib.gfz_HorizLine(x - xc, y + yc, 2 * xc);
}

template<typename T>
void gfz_internal_Ellipse_dual_line_NoClip(
    GraphZ<T>& lib, int32_t x, int32_t y, int32_t xc, int32_t yc
) {
    lib.gfz_HorizLine_NoClip(x - xc, (uint8_t)(y - yc), 2 * xc);
    lib.gfz_HorizLine_NoClip(x - xc, (uint8_t)(y + yc), 2 * xc);
}

// Derived from graphx.asm
template<typename T>
void gfz_internal_Ellipse(
    GraphZ<T>& lib, int32_t x, int32_t y, uint32_t a, uint32_t b,
    void (*plot_function)(GraphZ<T>&, int32_t, int32_t, int32_t, int32_t)
) {
    int32_t a2 = a * a;
    int32_t fa2 = 4 * a2;
    if (a == 0 || b == 0) {
        return;
    }
    int32_t yc = b;
    int32_t sigma_add_1 = fa2 * (1 - b);
    int32_t b2 = b * b;
    int32_t fb2 = 4 * b2;
    int32_t xc = 0;
    int32_t sigma = 2 * b2 + a2 * (1 - 2 * b);
    int32_t sigma_add_2 = fb2 * (1 - a);

    while(b2 * xc <= a2 * yc) { /* .main_loop1 */
        (plot_function)(lib, x, y, xc, yc);
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
        (plot_function)(lib, x, y, xc, yc);
        if (sigma >= 0) {
            sigma += sigma_add_2;
            sigma_add_2 += fb2;
            xc--;
        }

        sigma += a2 * (4 * yc + 6);
        yc++;
    }
}

template<typename T>
void GraphZ<T>::gfz_Ellipse(int32_t x, int32_t y, uint32_t a, uint32_t b) {
    gfz_internal_Ellipse(
        *this, x, y, a, b,
        &gfz_internal_Ellipse_dual_point
    );
}

template<typename T>
void GraphZ<T>::gfz_Ellipse_NoClip(uint32_t x, uint32_t y, uint8_t a, uint8_t b) {
    gfz_internal_Ellipse(
        *this, x, y, a, b,
        &gfz_internal_Ellipse_dual_point_NoClip
    );
}

template<typename T>
void GraphZ<T>::gfz_FillEllipse(int32_t x, int32_t y, uint32_t a, uint32_t b) {
    gfz_internal_Ellipse(
        *this, x, y, a, b,
        &gfz_internal_Ellipse_dual_line
    );
}

template<typename T>
void GraphZ<T>::gfz_FillEllipse_NoClip(uint32_t x, uint32_t y, uint8_t a, uint8_t b) {
    gfz_internal_Ellipse(
        *this, x, y, a, b,
        &gfz_internal_Ellipse_dual_line_NoClip
    );
}

//------------------------------------------------------------------------------
// Triangles
//------------------------------------------------------------------------------

// y2 >= y1 >= y0
inline void gfz_internal_triangle_sort(
    int32_t& x0, int32_t& y0,
    int32_t& x1, int32_t& y1,
    int32_t& x2, int32_t& y2
) {
    if (y0 > y1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    if (y0 > y1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
}

template<typename T>
void GraphZ<T>::gfz_FillTriangle(
    int32_t x0, int32_t y0,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2
) {
    gfz_internal_triangle_sort(
        x0, y0,
        x1, y1,
        x2, y2
    );
    if (y0 == y2) { // Degenerate Triangle
        int32_t x_min = std::min({x0, x1, x2});
        int32_t x_max = std::max({x0, x1, x2});
        gfz_HorizLine(x_min, y0, x_max - x_min + 1);
        return;
    }
    int32_t sa = 0;
    int32_t sb = 0;
    int32_t dx01 = x1 - x0;
    int32_t dx02 = x2 - x0;
    int32_t dy01 = y1 - y0;
    int32_t dy02 = y2 - y0;
    int32_t dx12 = x2 - x1;
    int32_t dy12 = y2 - y1;
    int32_t last;
    if (y1 == y2) {
        last = y1;
    } else {
        last = y1 - 1;
    }
    int32_t y;
    for (y = y0; y <= last; y++) {
        int32_t a = x0 + sa / dy01;
        int32_t b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (b < a) { // Swap
            int32_t temp = a;
            a = b;
            b = temp;
        }
        gfz_HorizLine(a, y, b - a + 1);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y <= y2; y++) {
        int32_t a = x1 + sa / dy12;
        int32_t b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (b < a) { // Swap
            int32_t temp = a;
            a = b;
            b = temp;
        }
        gfz_HorizLine(a, y, b - a + 1);
    }
}

template<typename T>
void GraphZ<T>::gfz_FillTriangle_NoClip(
    int32_t x0, int32_t y0,
    int32_t x1, int32_t y1,
    int32_t x2, int32_t y2
) {
    gfz_internal_triangle_sort(
        x0, y0,
        x1, y1,
        x2, y2
    );
    if (y0 == y2) { // Degenerate Triangle
        int32_t x_min = std::min({x0, x1, x2});
        int32_t x_max = std::max({x0, x1, x2});
        gfz_HorizLine_NoClip(x_min, (uint8_t)y0, x_max - x_min + 1);
        return;
    }
    int32_t sa = 0;
    int32_t sb = 0;
    int32_t dx01 = x1 - x0;
    int32_t dx02 = x2 - x0;
    int32_t dy01 = y1 - y0;
    int32_t dy02 = y2 - y0;
    int32_t dx12 = x2 - x1;
    int32_t dy12 = y2 - y1;
    int32_t last;
    if (y1 == y2) {
        last = y1;
    } else {
        last = y1 - 1;
    }
    int32_t y;
    for (y = y0; y <= last; y++) {
        int32_t a = x0 + sa / dy01;
        int32_t b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (b < a) { // Swap
            int32_t temp = a;
            a = b;
            b = temp;
        }
        gfz_HorizLine_NoClip(a, (uint8_t)y, b - a + 1);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y <= y2; y++) {
        int32_t a = x1 + sa / dy12;
        int32_t b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (b < a) { // Swap
            int32_t temp = a;
            a = b;
            b = temp;
        }
        gfz_HorizLine_NoClip(a, (uint8_t)y, b - a + 1);
    }
}

//------------------------------------------------------------------------------
// Unfilled Polygons
//------------------------------------------------------------------------------

template<typename T>
void GraphZ<T>::gfz_Polygon(const ti_int *points, size_t num_points) {
    if (num_points < 2) {
        return;
    }
    gfz_Line(
        (int24_t)points[num_points * 2 - 2], (int24_t)points[num_points * 2 - 1],
        (int24_t)points[0], (int24_t)points[1]
    );
    for (size_t i = 0; i < 2 * num_points - 2; i += 2) {
        gfz_Line(
            (int24_t)points[i + 0], (int24_t)points[i + 1],
            (int24_t)points[i + 2], (int24_t)points[i + 3]
        );
    }
}

template<typename T>
void GraphZ<T>::gfz_Polygon_NoClip(const ti_int *points, size_t num_points) {
    if (num_points < 2) {
        return;
    }
    gfz_Line_NoClip(
        (int24_t)points[num_points * 2 - 2], (int24_t)points[num_points * 2 - 1],
        (int24_t)points[0], (int24_t)points[1]
    );
    for (size_t i = 0; i < 2 * num_points - 2; i += 2) {
        gfz_Line_NoClip(
            (int24_t)points[i + 0], (int24_t)points[i + 1],
            (int24_t)points[i + 2], (int24_t)points[i + 3]
        );
    }
}

//------------------------------------------------------------------------------
// Floodfill
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Sprite Utilities
//------------------------------------------------------------------------------

template<typename T>
gfz_sprite_t *GraphZ<T>::gfz_AllocSprite(
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

//------------------------------------------------------------------------------
// Sprite Othogonal Transformations
//------------------------------------------------------------------------------

template<typename T>
gfz_sprite_t *GraphZ<T>::gfz_RotateSpriteHalf(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
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

//------------------------------------------------------------------------------
// Scaled Sprites
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Sprite Rotation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Tilemaps
//------------------------------------------------------------------------------

template<typename T>
void GraphZ<T>::gfz_Tilemap(const gfz_tilemap_t* tilemap, uint32_t x_offset, uint32_t y_offset) {
    int32_t x_draw, y_draw;
    uint8_t x, x_tile, y_tile, y_next;
    uint8_t x_res = x_offset / tilemap->tile_width;
    uint8_t y = y_offset / tilemap->tile_height;

    x_offset = x_offset % tilemap->tile_width;
    y_offset = y_offset % tilemap->tile_height;

    y_draw = tilemap->y_loc - y_offset;
    for (y_tile = 0; y_tile <= tilemap->draw_height; y_tile++) {
        x = x_res;
        y_next = y * tilemap->width;
        x_draw = tilemap->x_loc - x_offset;
        for (x_tile = 0; x_tile <= tilemap->draw_width; x_tile++) {
            gfz_Sprite(
                tilemap->tiles[tilemap->map[x + y_next]],
                x_draw,
                y_draw
            );
            x_draw += tilemap->tile_width;
            x++;
        }
        y_draw += tilemap->tile_height;
        y++;
    }
}

template<typename T>
void GraphZ<T>::gfz_Tilemap_NoClip(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset) {
    int32_t x_draw, y_draw;
    uint8_t x, x_tile, y_tile, y_next;
    uint8_t x_res = x_offset / tilemap->tile_width;
    uint8_t y = y_offset / tilemap->tile_height;

    x_offset = x_offset % tilemap->tile_width;
    y_offset = y_offset % tilemap->tile_height;

    y_draw = tilemap->y_loc - y_offset;
    for (y_tile = 0; y_tile <= tilemap->draw_height; y_tile++) {
        x = x_res;
        y_next = y * tilemap->width;
        x_draw = tilemap->x_loc - x_offset;
        for (x_tile = 0; x_tile <= tilemap->draw_width; x_tile++) {
            gfz_Sprite_NoClip(
                tilemap->tiles[tilemap->map[x + y_next]],
                x_draw,
                y_draw
            );
            x_draw += tilemap->tile_width;
            x++;
        }
        y_draw += tilemap->tile_height;
        y++;
    }
}

template<typename T>
void GraphZ<T>::gfz_TransparentTilemap(const gfz_tilemap_t* tilemap, uint32_t x_offset, uint32_t y_offset) {
    int32_t x_draw, y_draw;
    uint8_t x, x_tile, y_tile, y_next;
    uint8_t x_res = x_offset / tilemap->tile_width;
    uint8_t y = y_offset / tilemap->tile_height;

    x_offset = x_offset % tilemap->tile_width;
    y_offset = y_offset % tilemap->tile_height;

    y_draw = tilemap->y_loc - y_offset;
    for (y_tile = 0; y_tile <= tilemap->draw_height; y_tile++) {
        x = x_res;
        y_next = y * tilemap->width;
        x_draw = tilemap->x_loc - x_offset;
        for (x_tile = 0; x_tile <= tilemap->draw_width; x_tile++) {
            gfz_TransparentSprite(
                tilemap->tiles[tilemap->map[x + y_next]],
                x_draw,
                y_draw
            );
            x_draw += tilemap->tile_width;
            x++;
        }
        y_draw += tilemap->tile_height;
        y++;
    }
}

template<typename T>
void GraphZ<T>::gfz_TransparentTilemap_NoClip(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset) {
    int32_t x_draw, y_draw;
    uint8_t x, x_tile, y_tile, y_next;
    uint8_t x_res = x_offset / tilemap->tile_width;
    uint8_t y = y_offset / tilemap->tile_height;

    x_offset = x_offset % tilemap->tile_width;
    y_offset = y_offset % tilemap->tile_height;

    y_draw = tilemap->y_loc - y_offset;
    for (y_tile = 0; y_tile <= tilemap->draw_height; y_tile++) {
        x = x_res;
        y_next = y * tilemap->width;
        x_draw = tilemap->x_loc - x_offset;
        for (x_tile = 0; x_tile <= tilemap->draw_width; x_tile++) {
            gfz_TransparentSprite_NoClip(
                tilemap->tiles[tilemap->map[x + y_next]],
                x_draw,
                y_draw
            );
            x_draw += tilemap->tile_width;
            x++;
        }
        y_draw += tilemap->tile_height;
        y++;
    }
}

/** @todo Validate that this is correct for graphx */
template<typename T>
uint8_t *GraphZ<T>::gfz_TilePtr(const gfz_tilemap_t *tilemap, uint32_t x_offset, uint32_t y_offset) {
    uint32_t map_row = x_offset / tilemap->tile_width;
    uint32_t map_col = y_offset / tilemap->tile_height;

    uint32_t map_index = map_row + (map_col * tilemap->width);
    return &(tilemap->map[map_index]);
}

/** @todo Validate that this is correct for graphx */
template<typename T>
uint8_t *GraphZ<T>::gfz_TilePtrMapped(const gfz_tilemap_t *tilemap, uint8_t col, uint8_t row) {
    uint32_t map_index = row + (col * tilemap->width);
    return &(tilemap->map[map_index]);
}

//------------------------------------------------------------------------------
// RLET Sprites
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------


#endif /* GRAPHZ_HPP */
