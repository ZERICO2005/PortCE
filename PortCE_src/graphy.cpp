#include <PortCE.h>

#include <graphy.h>

#include "graphz.hpp"
#include "graphy_data.h"

#define lcd_Control              (*(volatile uint24_t*)RAM_ADDRESS(0xE30018))
#define lcd_VideoMode            (*(volatile uint16_t*)RAM_ADDRESS(0xE30018))
#define lcd_UpBase               (*(volatile uint24_t*)RAM_ADDRESS(0xE30010))
#define lcd_LpBase               (*(volatile uint24_t*)RAM_ADDRESS(0xE30014))
#define lcd_BGR8bit 0x927
#define lcd_BGR16bit 0x92D

struct GraphY_Type {
    using region = gfy_region_t;
    using sprite = gfy_sprite_t;
    using rletsprite = gfy_rletsprite_t;
    using tilemap_type = gfy_tilemap_type_t;
    using tilemap = gfy_tilemap_t;
    using mode = gfy_mode_t;
    using location = gfy_location_t;
    using text_options = gfy_text_options_t;
};

typedef GraphZ<GraphY_Type> GraphY;

template<>
void GraphY::gfz_SetPixel_NoClip(ti_unsigned_int x, uint8_t y, uint8_t color) {
    if (x < GFY_LCD_WIDTH && y < GFY_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint24_t)x + (y * GFY_LCD_WIDTH)] = color;
    }
}

static GraphY lib("graphy", gfy_DefaultCharSpacing, gfy_DefaultTextData);

//------------------------------------------------------------------------------
// Wrapper Functions
//------------------------------------------------------------------------------

void gfy_Begin(void) {
    lib.gfz_Begin();
}

void gfy_End(void) {
    lib.gfz_End();
}

gfy_sprite_t *gfy_AllocSprite(
    uint8_t width,
    uint8_t height,
    void *(*malloc_routine)(size_t)
) {
    return lib.gfz_AllocSprite(width, height, malloc_routine);
}

void gfy_Tilemap(
    const gfy_tilemap_t *tilemap,
    ti_unsigned_int x_offset,
    ti_unsigned_int y_offset
) {
    lib.gfz_Tilemap(
        tilemap,
        static_cast<uint32_t>(x_offset),
        static_cast<uint32_t>(y_offset)
    );
}

void gfy_Tilemap_NoClip(
    const gfy_tilemap_t *tilemap,
    ti_unsigned_int x_offset,
    ti_unsigned_int y_offset
) {
    lib.gfz_Tilemap_NoClip(
        tilemap,
        static_cast<uint32_t>(x_offset),
        static_cast<uint32_t>(y_offset)
    );
}

void gfy_TransparentTilemap(
    const gfy_tilemap_t *tilemap,
    ti_unsigned_int x_offset,
    ti_unsigned_int y_offset
) {
    lib.gfz_TransparentTilemap(
        tilemap,
        static_cast<uint32_t>(x_offset),
        static_cast<uint32_t>(y_offset)
    );
}

void gfy_TransparentTilemap_NoClip(
    const gfy_tilemap_t *tilemap,
    ti_unsigned_int x_offset,
    ti_unsigned_int y_offset
) {
    lib.gfz_TransparentTilemap_NoClip(
        tilemap,
        static_cast<uint32_t>(x_offset),
        static_cast<uint32_t>(y_offset)
    );
}

uint8_t *gfy_TilePtr(
    const gfy_tilemap_t *tilemap,
    ti_unsigned_int x_offset,
    ti_unsigned_int y_offset
) {
    return lib.gfz_TilePtr(tilemap, static_cast<uint32_t>(x_offset), y_offset);
}

uint8_t *gfy_TilePtrMapped(
    const gfy_tilemap_t *tilemap,
    uint8_t col,
    uint8_t row
) {
    return lib.gfz_TilePtrMapped(tilemap, col, row);
}

uint8_t gfy_SetColor(uint8_t index) {
    return lib.gfz_SetColor(index);
}

uint8_t gfy_SetTransparentColor(uint8_t index) {
    return lib.gfz_SetTransparentColor(index);
}

void gfy_SetDefaultPalette(gfy_mode_t mode) {
    lib.gfz_SetDefaultPalette(mode);
}

void gfy_SetPalette(const void *palette, size_t size, uint8_t offset) {
    lib.gfz_SetPalette(palette, size, offset);
}

void gfy_FillScreen(uint8_t index) {
    lib.gfz_FillScreen(index);
}

void gfy_ZeroScreen(void) {
    lib.gfz_ZeroScreen();
}

void gfy_SetPixel(ti_unsigned_int x, uint8_t y) {
    lib.gfz_SetPixel(static_cast<uint32_t>(x), y);
}

uint8_t gfy_GetPixel(ti_unsigned_int x, uint8_t y) {
    return lib.gfz_GetPixel(static_cast<uint32_t>(x), y);
}

void gfy_Line(
    ti_int x0,ti_int y0,
    ti_int x1, ti_int y1
) {
    lib.gfz_Line(
        static_cast<int32_t>(x0), static_cast<int32_t>(y0),
        static_cast<int32_t>(x1), static_cast<int32_t>(y1)
    );
}

void gfy_Line_NoClip(
    ti_unsigned_int x0, uint8_t y0,
    ti_unsigned_int x1, uint8_t y1
) {
    lib.gfz_Line_NoClip(
        static_cast<uint32_t>(x0), y0,
        static_cast<uint32_t>(x1), y1
    );
}

void gfy_HorizLine(ti_int x, ti_int y, ti_int length) {
    lib.gfz_HorizLine(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<int32_t>(length)
    );
}

void gfy_HorizLine_NoClip(ti_unsigned_int x, uint8_t y, ti_unsigned_int length) {
    lib.gfz_HorizLine_NoClip(
        static_cast<uint32_t>(x), y,
        static_cast<uint32_t>(length)
    );
}

void gfy_VertLine(ti_int x, ti_int y, ti_int length) {
    lib.gfz_VertLine(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<int32_t>(length)
    );
}

void gfy_VertLine_NoClip(ti_unsigned_int x, uint8_t y, ti_unsigned_int length) {
    lib.gfz_VertLine_NoClip(
        static_cast<uint32_t>(x), y,
        static_cast<uint32_t>(length)
    );
}

void gfy_Rectangle(
    ti_int x, ti_int y,
    ti_int width, ti_int height
) {
    lib.gfz_Rectangle(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<int32_t>(width), static_cast<int32_t>(height)
    );
}

void gfy_Rectangle_NoClip(
    ti_unsigned_int x, uint8_t y,
    ti_unsigned_int width, uint8_t height
) {
    lib.gfz_Rectangle_NoClip(
        static_cast<uint32_t>(x), y,
        static_cast<uint32_t>(width), height
    );
}

void gfy_FillRectangle(
    ti_int x, ti_int y,
    ti_int width, ti_int height
) {
    lib.gfz_FillRectangle(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<int32_t>(width), static_cast<int32_t>(height)
    );
}

void gfy_FillRectangle_NoClip(
    ti_unsigned_int x, uint8_t y,
    ti_unsigned_int width, uint8_t height
) {
    lib.gfz_FillRectangle_NoClip(
        static_cast<uint32_t>(x), y,
        static_cast<uint32_t>(width), height
    );
}

void gfy_Circle(
    ti_int x, ti_int y,
    ti_unsigned_int radius
) {
    lib.gfz_Circle(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<uint32_t>(radius)
    );
}

void gfy_FillCircle(
    ti_int x, ti_int y,
    ti_unsigned_int radius
) {
    lib.gfz_FillCircle(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<uint32_t>(radius)
    );
}

void gfy_FillCircle_NoClip(
    ti_unsigned_int x, uint8_t y,
    ti_unsigned_int radius
) {
    lib.gfz_FillCircle_NoClip(
        static_cast<uint32_t>(x), y,
        static_cast<uint32_t>(radius)
    );
}

void gfy_FillEllipse_NoClip(
    ti_unsigned_int x, ti_unsigned_int y,
    uint8_t a, uint8_t b
) {
    lib.gfz_FillEllipse_NoClip(
        static_cast<uint32_t>(x), static_cast<uint32_t>(y),
        a, b
    );
}

void gfy_FillEllipse(
    ti_int x, ti_int y,
    ti_unsigned_int a, ti_unsigned_int b
) {
    lib.gfz_FillEllipse(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<uint32_t>(a), static_cast<uint32_t>(b)
    );
}

void gfy_Ellipse_NoClip(
    ti_unsigned_int x, ti_unsigned_int y,
    uint8_t a, uint8_t b
) {
    lib.gfz_Ellipse_NoClip(
        static_cast<uint32_t>(x), static_cast<uint32_t>(y),
        a, b
    );
}

void gfy_Ellipse(
    ti_int x, ti_int y,
    ti_unsigned_int a, ti_unsigned_int b
) {
    lib.gfz_Ellipse(
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        static_cast<uint32_t>(a), static_cast<uint32_t>(b)
    );
}

void gfy_Polygon(const ti_int *points, size_t num_points) {
    lib.gfz_Polygon(points, num_points);
}

void gfy_Polygon_NoClip(const ti_int *points, size_t num_points) {
    lib.gfz_Polygon_NoClip(points, num_points);
}

void gfy_FillTriangle(
    ti_int x0, ti_int y0,
    ti_int x1, ti_int y1,
    ti_int x2, ti_int y2
) {
    lib.gfz_FillTriangle(
        static_cast<int32_t>(x0), static_cast<int32_t>(y0),
        static_cast<int32_t>(x1), static_cast<int32_t>(y1),
        static_cast<int32_t>(x2), static_cast<int32_t>(y2)
    );
}

void gfy_FillTriangle_NoClip(
    ti_int x0, ti_int y0,
    ti_int x1, ti_int y1,
    ti_int x2, ti_int y2
) {
    lib.gfz_FillTriangle_NoClip(
        static_cast<int32_t>(x0), static_cast<int32_t>(y0),
        static_cast<int32_t>(x1), static_cast<int32_t>(y1),
        static_cast<int32_t>(x2), static_cast<int32_t>(y2)
    );
}

void gfy_SetDraw(uint8_t location) {
    lib.gfz_SetDraw(location);
}

uint8_t gfy_GetDraw(void) {
    return lib.gfz_GetDraw();
}

void gfy_SwapDraw(void) {
    lib.gfz_SwapDraw();
}

void gfy_Wait(void) {
    lib.gfz_Wait();
}

void gfy_Blit(gfy_location_t src) {
    lib.gfz_Blit(src);
}

void gfy_BlitColumns(gfy_location_t src, uint8_t y_loc, uint8_t num_lines) {
    lib.gfz_BlitColumns(src, y_loc, num_lines);
}

void gfy_BlitRectangle(
    gfy_location_t src,
    ti_unsigned_int x, uint8_t y,
    ti_unsigned_int width, ti_unsigned_int height
) {
    lib.gfz_BlitRectangle(
        src,
        static_cast<uint32_t>(x),
        y,
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    );
}

void gfy_CopyRectangle(
    gfy_location_t src, gfy_location_t dst,
    ti_unsigned_int src_x, uint8_t src_y,
    ti_unsigned_int dst_x, uint8_t dst_y,
    ti_unsigned_int width, uint8_t height
) {
    lib.gfz_CopyRectangle(
        src, dst,
        static_cast<uint32_t>(src_x), src_y,
        static_cast<uint32_t>(dst_x), dst_y,
        static_cast<uint32_t>(width), height
    );
}

void gfy_SetTextScale(uint8_t width_scale, uint8_t height_scale) {
    lib.gfz_SetTextScale(width_scale, height_scale);
}

void gfy_PrintChar(const char c) {
    lib.gfz_PrintChar(c);
}

void gfy_PrintInt(int24_t n, uint8_t length) {
    lib.gfz_PrintInt(n, length);
}

void gfy_PrintUInt(uint24_t n, uint8_t length) {
    lib.gfz_PrintUInt(n, length);
}

void gfy_PrintString(const char *string) {
    lib.gfz_PrintString(string);
}

void gfy_PrintStringXY(const char *string, ti_int x, ti_int y) {
    lib.gfz_PrintStringXY(
        string,
        static_cast<int32_t>(x), static_cast<int32_t>(y)
    );
}

ti_int gfy_GetTextX(void) {
    return lib.gfz_GetTextX();
}

ti_int gfy_GetTextY(void) {
    return lib.gfz_GetTextY();
}

void gfy_SetTextXY(ti_int x, ti_int y) {
    lib.gfz_SetTextXY(static_cast<int32_t>(x), static_cast<int32_t>(y));
}

void gfy_SetTextConfig(uint8_t config) {
    lib.gfz_SetTextConfig(config);
}

uint8_t gfy_SetTextFGColor(uint8_t color) {
    return lib.gfz_SetTextFGColor(color);
}

uint8_t gfy_SetTextBGColor(uint8_t color) {
    return lib.gfz_SetTextBGColor(color);
}

uint8_t gfy_SetTextTransparentColor(uint8_t color) {
    return lib.gfz_SetTextTransparentColor(color);
}

void gfy_Sprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_Sprite(
        sprite,
        static_cast<int32_t>(x), static_cast<int32_t>(y)
    );
}

void gfy_Sprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_unsigned_int x, uint8_t y
) {
    lib.gfz_Sprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y
    );
}

void gfy_TransparentSprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_TransparentSprite(
        sprite,
        static_cast<int32_t>(x), static_cast<int32_t>(y)
    );
}

void gfy_TransparentSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_unsigned_int x,uint8_t y
) {
    lib.gfz_TransparentSprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y
    );
}

gfy_sprite_t *gfy_GetSprite(
    gfy_sprite_t *sprite_buffer,
    ti_int x, ti_int y
) {
    return lib.gfz_GetSprite(
        sprite_buffer,
        static_cast<int32_t>(x), static_cast<int32_t>(y)
    );
}

void gfy_ScaledSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_unsigned_int x, uint8_t y,
    uint8_t width_scale, uint8_t height_scale
) {
    lib.gfz_ScaledSprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y,
        width_scale, height_scale
    );
}

void gfy_ScaledTransparentSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_unsigned_int x, uint8_t y,
    uint8_t width_scale, uint8_t height_scale
) {
    lib.gfz_ScaledTransparentSprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y,
        width_scale, height_scale
    );
}

uint8_t gfy_RotatedScaledTransparentSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_unsigned_int x, uint8_t y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledTransparentSprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y,
        angle, scale
    );
}

uint8_t gfy_RotatedScaledSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_unsigned_int x, uint8_t y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledSprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y,
        angle, scale
    );
}

uint8_t gfy_RotatedScaledTransparentSprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledTransparentSprite(
        sprite,
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        angle, scale
    );
}

uint8_t gfy_RotatedScaledSprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledSprite(
        sprite,
        static_cast<int32_t>(x), static_cast<int32_t>(y),
        angle, scale
    );
}

gfy_sprite_t *gfy_FlipSpriteX(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return lib.gfz_FlipSpriteX(sprite_in, sprite_out);
}

gfy_sprite_t *gfy_FlipSpriteY(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return lib.gfz_FlipSpriteY(sprite_in, sprite_out);
}

gfy_sprite_t *gfy_RotateSpriteC(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return lib.gfz_RotateSpriteC(sprite_in, sprite_out);
}

gfy_sprite_t *gfy_RotateSpriteCC(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return lib.gfz_RotateSpriteCC(sprite_in, sprite_out);
}

gfy_sprite_t *gfy_RotateSpriteHalf(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return lib.gfz_RotateSpriteHalf(sprite_in, sprite_out);
}

gfy_sprite_t *gfy_ScaleSprite(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return lib.gfz_ScaleSprite(sprite_in, sprite_out);
}

gfy_sprite_t *gfy_RotateScaleSprite(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out,
    uint8_t angle,
    uint8_t scale
) {
    return lib.gfz_RotateScaleSprite(sprite_in, sprite_out, angle, scale);
}

gfy_sprite_t *gfy_GetSpriteChar(char c) {
    return lib.gfz_GetSpriteChar(c);
}

uint8_t *gfy_SetFontData(const uint8_t *data) {
    return lib.gfz_SetFontData(data);
}

uint8_t *gfy_SetCharData(uint8_t index, const uint8_t *data) {
    return lib.gfz_SetCharData(index, data);
}

void gfy_SetFontSpacing(const uint8_t *spacing) {
    lib.gfz_SetFontSpacing(spacing);
}

uint8_t gfy_SetFontHeight(uint8_t height) {
    return lib.gfz_SetFontHeight(height);
}

void gfy_SetMonospaceFont(uint8_t spacing) {
    lib.gfz_SetMonospaceFont(spacing);
}

ti_unsigned_int gfy_GetStringWidth(const char *string) {
    return lib.gfz_GetStringWidth(string);
}

ti_unsigned_int gfy_GetCharWidth(const char c) {
    return lib.gfz_GetCharWidth(c);
}

void gfy_SetClipRegion(ti_int xmin, ti_int ymin, ti_int xmax, ti_int ymax) {
    lib.gfz_SetClipRegion(
        static_cast<int32_t>(xmin),
        static_cast<int32_t>(ymin),
        static_cast<int32_t>(xmax),
        static_cast<int32_t>(ymax)
    );
}

bool gfy_GetClipRegion(gfy_region_t *region) {
    return lib.gfz_GetClipRegion(region);
}

void gfy_ShiftDown(uint8_t pixels) {
    lib.gfz_ShiftDown(pixels);
}

void gfy_ShiftUp(uint8_t pixels) {
    lib.gfz_ShiftUp(pixels);
}

void gfy_ShiftLeft(ti_unsigned_int pixels) {
    lib.gfz_ShiftLeft(static_cast<uint32_t>(pixels));
}

void gfy_ShiftRight(ti_unsigned_int pixels) {
    lib.gfz_ShiftRight(static_cast<uint32_t>(pixels));
}

uint16_t gfy_Lighten(uint16_t color, uint8_t amount) {
    return lib.gfz_Lighten(color, amount);
}

uint16_t gfy_Darken(uint16_t color, uint8_t amount) {
    return lib.gfz_Darken(color, amount);
}

void gfy_FloodFill(ti_unsigned_int x, uint8_t y, uint8_t color) {
    lib.gfz_FloodFill(static_cast<uint32_t>(x), y, color);
}

void gfy_RLETSprite(
    const gfy_rletsprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_RLETSprite(
        sprite,
        static_cast<int32_t>(x), static_cast<int32_t>(y)
    );
}

void gfy_RLETSprite_NoClip(
    const gfy_rletsprite_t *sprite,
    ti_unsigned_int x, uint8_t y
) {
    lib.gfz_RLETSprite_NoClip(
        sprite,
        static_cast<uint32_t>(x), y
    );
}

gfy_sprite_t *gfy_ConvertFromRLETSprite(
    const gfy_rletsprite_t *sprite_in,
    gfy_sprite_t *sprite_out
) {
    return lib.gfz_ConvertFromRLETSprite(sprite_in, sprite_out);
}

gfy_rletsprite_t *gfy_ConvertToRLETSprite(
    const gfy_sprite_t *sprite_in,
    gfy_rletsprite_t *sprite_out
) {
    return lib.gfz_ConvertToRLETSprite(sprite_in, sprite_out);
}

gfy_rletsprite_t *gfy_ConvertToNewRLETSprite(
    const gfy_sprite_t *sprite_in,
    void *(*malloc_routine)(size_t)
) {
    return lib.gfz_ConvertToNewRLETSprite(sprite_in, malloc_routine);
}

//------------------------------------------------------------------------------
// Code
//------------------------------------------------------------------------------
