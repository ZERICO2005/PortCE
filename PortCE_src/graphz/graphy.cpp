#include <PortCE.h>

#include <graphy.h>

#include "lib_graphz.hpp"
#include "graphy_data.h"

#include <lcddrvce.h>

struct GraphY_Type {};

typedef GraphZ<GraphY_Type> GraphY;

static GraphY lib("graphy", graphy_DefaultCharSpacing, graphy_DefaultTextData);

//------------------------------------------------------------------------------
// Rendering Primitives
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_Begin(void) {
    lcd_Init();
    util_Begin(*this);
    lcd_VideoMode = lcd_BGR8bit;
    lcd_SetRamAccessOrder(LCD_MADCTL_DEFAULT ^ LCD_MV);
    lcd_SetColumnAddress(0, 239);
    lcd_SetRowAddress(0, 319);
    lcd_Cleanup();
}

template<>
void GraphY::gfz_End(void) {
    lcd_Init();
    lcd_SetRamAccessOrder(LCD_MADCTL_DEFAULT);
    lcd_SetColumnAddress(0, 319);
    lcd_SetRowAddress(0, 239);
    lcd_Cleanup();
    lcd_VideoMode = lcd_BGR16bit;
    util_End(*this);
}

//------------------------------------------------------------------------------
// Clipping
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Blitting
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_BlitColumns(gfz_location_t src, uint32_t x_loc, uint32_t num_columns) {
    const uint8_t *src_buf = gfz_vram;
    uint8_t *dst_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
    if (src) {
        src_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
        dst_buf = gfz_vram;
    }
    src_buf += x_loc * GFZ_LCD_HEIGHT;
    dst_buf += x_loc * GFZ_LCD_HEIGHT;
    memcpy(dst_buf, src_buf, num_columns * GFZ_LCD_HEIGHT);
}

template<>
void GraphY::gfz_BlitRectangle(
    gfz_location_t src,
    uint32_t x,
    uint8_t y,
    uint32_t width,
    uint32_t height
) {
    const uint8_t *src_buf = gfz_vram;
    uint8_t *dst_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
    if (src) {
        src_buf = gfz_vram + (GFZ_LCD_HEIGHT * GFZ_LCD_WIDTH);
        dst_buf = gfz_vram;
    }
    src_buf += y + (x * GFZ_LCD_HEIGHT);
    dst_buf += y + (x * GFZ_LCD_HEIGHT);
    for (uint32_t x_cord = 0; x_cord < width; x_cord++) {
        memcpy(dst_buf, src_buf, height);
        src_buf += GFZ_LCD_HEIGHT;
        dst_buf += GFZ_LCD_HEIGHT;
    }
}

template<>
void GraphY::gfz_CopyRectangle(
    gfz_location_t src, gfz_location_t dst,
    uint32_t src_x, uint8_t src_y,
    uint32_t dst_x, uint8_t dst_y,
    uint32_t width, uint8_t height
) {
    const uint8_t* src_buf = (src == gfz_screen) ?
        (uint8_t*)RAM_ADDRESS(lcd_UpBase) : (uint8_t*)RAM_ADDRESS(lcd_LpBase);
    uint8_t* dst_buf = (dst == gfz_screen) ?
        (uint8_t*)RAM_ADDRESS(lcd_UpBase) : (uint8_t*)RAM_ADDRESS(lcd_LpBase);
    const bool buf_overlap = (src_buf == dst_buf) ? true : false;
    src_buf += src_y + (src_x * GFZ_LCD_HEIGHT);
    dst_buf += dst_y + (dst_x * GFZ_LCD_HEIGHT);
    const uint32_t jump = GFZ_LCD_HEIGHT - height;
    if (buf_overlap == true) {
        for (uint32_t x_cord = 0; x_cord < width; x_cord++) {
            memmove(dst_buf, src_buf, height);
            src_buf += jump;
            dst_buf += jump;
        }
        return;
    }
    for (uint32_t x_cord = 0; x_cord < width; x_cord++) {
        memcpy(dst_buf, src_buf, height);
        src_buf += jump;
        dst_buf += jump;
    }
}

//------------------------------------------------------------------------------
// Screen Shifting
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_ShiftRight(uint32_t pixels) {
    if (pixels == 0) { return; }
    int32_t shift = (int32_t)(uint32_t)pixels;
    int32_t y0 = lib.ClipYMin;
    int32_t y1 = lib.ClipYMax;
    int32_t x0 = lib.ClipXMin + shift;
    int32_t x1 = lib.ClipXMax;
    if (x1 <= x0 || y1 <= y0) {
        return;
    }
    size_t copy_size = y1 - y0;
    size_t line_count = x1 - x0;

    uint8_t * buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y0 + ((x1 - 1) * GFZ_LCD_HEIGHT);
    uint8_t const * src = buf - (shift * GFZ_LCD_HEIGHT);
    uint8_t       * dst = buf;
    for (size_t i = 0; i < line_count; i++) {
        memcpy(dst, src, copy_size);
        src -= GFZ_LCD_HEIGHT;
        dst -= GFZ_LCD_HEIGHT;
    }
}

template<>
void GraphY::gfz_ShiftLeft(uint32_t pixels) {
    if (pixels == 0) { return; }
    int32_t shift = (int32_t)(uint32_t)pixels;
    int32_t y0 = lib.ClipYMin;
    int32_t y1 = lib.ClipYMax;
    int32_t x0 = lib.ClipXMin;
    int32_t x1 = lib.ClipXMax - shift;
    if (x1 <= x0 || y1 <= y0) {
        return;
    }
    size_t copy_size = y1 - y0;
    size_t line_count = x1 - x0;

    uint8_t * buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y0 + (x0 * GFZ_LCD_HEIGHT);
    uint8_t const * src = buf + (shift * GFZ_LCD_HEIGHT);
    uint8_t       * dst = buf;
    for (size_t i = 0; i < line_count; i++) {
        memcpy(dst, src, copy_size);
        src += GFZ_LCD_HEIGHT;
        dst += GFZ_LCD_HEIGHT;
    }
}

template<>
void GraphY::gfz_ShiftDown(uint8_t pixels) {
    if (pixels == 0) { return; }
    int32_t shift = (int32_t)(uint32_t)pixels;
    int32_t y0 = lib.ClipYMin + shift;
    int32_t y1 = lib.ClipYMax;
    int32_t x0 = lib.ClipXMin;
    int32_t x1 = lib.ClipXMax;
    if (x1 <= x0 || y1 <= y0) {
        return;
    }
    size_t copy_size = y1 - y0;
    size_t line_count = x1 - x0;

    uint8_t * buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y0 + (x0 * GFZ_LCD_HEIGHT);
    uint8_t const * src = buf - shift;
    uint8_t       * dst = buf;
    for (size_t i = 0; i < line_count; i++) {
        memmove(dst, src, copy_size);
        src += GFZ_LCD_HEIGHT;
        dst += GFZ_LCD_HEIGHT;
    }
}

template<>
void GraphY::gfz_ShiftUp(uint8_t pixels) {
    if (pixels == 0) { return; }
    int32_t shift = (int32_t)(uint32_t)pixels;
    int32_t y0 = lib.ClipYMin;
    int32_t y1 = lib.ClipYMax - shift;
    int32_t x0 = lib.ClipXMin;
    int32_t x1 = lib.ClipXMax;
    if (x1 <= x0 || y1 <= y0) {
        return;
    }
    size_t copy_size = y1 - y0;
    size_t line_count = x1 - x0;

    uint8_t * buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y0 + (x0 * GFZ_LCD_HEIGHT);
    uint8_t const * src = buf + shift;
    uint8_t       * dst = buf;
    for (size_t i = 0; i < line_count; i++) {
        memmove(dst, src, copy_size);
        src += GFZ_LCD_HEIGHT;
        dst += GFZ_LCD_HEIGHT;
    }
}

//------------------------------------------------------------------------------
// Pixel Functions
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_SetPixel_NoClip(uint32_t x, uint8_t y, uint8_t color) {
    if (x < GFZ_LCD_WIDTH && y < GFZ_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[x * GFZ_LCD_HEIGHT + y] = color;
    }
}

template<>
void GraphY::gfz_SetPixel(uint32_t x, uint8_t y) {
    if (x < GFZ_LCD_WIDTH && y < GFZ_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[x * GFZ_LCD_HEIGHT + y] = lib.Color;
    }
}

template<>
uint8_t GraphY::gfz_GetPixel(uint32_t x, uint8_t y) {
    return ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(x & 0xFFFF) * GFZ_LCD_HEIGHT + y];
}

//------------------------------------------------------------------------------
// Screen Filling
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Horizontal Lines
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_HorizLine_NoClip(uint32_t x, uint8_t y, uint32_t length) {
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x * GFZ_LCD_HEIGHT) + (uint32_t)y;
    for (uint32_t x_cord = 0; x_cord < length; x_cord++) {
        *fill = lib.Color;
        fill += GFZ_LCD_HEIGHT;
    }
}

//------------------------------------------------------------------------------
// Vertical Lines
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_VertLine_NoClip(uint32_t x, uint8_t y, uint32_t length) {
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + ((uint32_t)y + (x * GFZ_LCD_HEIGHT));
    memset(fill, lib.Color, length);
}

//------------------------------------------------------------------------------
// Filled Rectangle
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_FillRectangle_NoClip(uint32_t x, uint8_t y, uint32_t width, uint8_t height) {
    if (width == 0 || height == 0) {
        return;
    }
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + ((uint32_t)y + (x * GFZ_LCD_HEIGHT));
    for (uint32_t x_cord = 0; x_cord < width; x_cord++) {
        memset(fill, lib.Color, height);
        fill += GFZ_LCD_HEIGHT;
    }
}

//------------------------------------------------------------------------------
// Rectangle Outline
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Text and Fonts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Character Drawing
//------------------------------------------------------------------------------

static void gfz_internal_PrintChar_NoClip(const char c, const uint8_t charWidth) {
    const uint8_t *bitImage = lib.TextData + lib.Maximum_Font_Height * (uint32_t)((unsigned char)c);
    uint8_t *fillLinePtr = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.TextYPos + (lib.TextXPos * GFZ_LCD_HEIGHT));

    lib.TextXPos += charWidth * lib.TextWidthScale;

    for (uint8_t x = 0; x < charWidth; x++) {
        for (uint8_t u = 0; u < lib.TextWidthScale; u++) {
            uint8_t *fillPtr = fillLinePtr;
            uint8_t b = (1 << 7);
            for (uint8_t y = 0; y < lib.FontHeight; y++) {
                const uint8_t fillColor = *bitImage & b ? lib.Text_FG_Color : lib.Text_BG_Color;
                b >>= 1;

                if (fillColor == lib.Text_TP_Color) {
                    fillPtr += lib.TextHeightScale;
                    continue;
                }

                for (uint8_t v = 0; v < lib.TextHeightScale; v++) {
                    *fillPtr = fillColor;
                    fillPtr++;
                }
            }
            fillLinePtr += GFZ_LCD_HEIGHT;
        }
        bitImage++;
    }
}

template<>
void GraphY::gfz_PrintChar(const char c) {
    const uint8_t charWidth = gfz_GetCharWidth(c);
    const uint8_t textSizeX = charWidth * lib.TextWidthScale;
    const uint8_t textSizeY = lib.Maximum_Font_Height * lib.TextHeightScale;
    if (
        lib.PrintChar_Clip == gfz_text_noclip ||
        /* Otherwise, if clipping is enabled */
        lib.TextXPos >= lib.ClipXMin || lib.TextYPos >= lib.ClipYMin ||
        lib.TextXPos + textSizeX <= lib.ClipXMax ||
        lib.TextYPos + textSizeY <= lib.ClipYMax
    ) {
        gfz_internal_PrintChar_NoClip(c, charWidth);
        return;
    }
    const uint8_t *bitImage = lib.TextData + lib.Maximum_Font_Height * (uint32_t)((unsigned char)c);
    uint8_t *fillLinePtr = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.TextYPos + (lib.TextXPos * GFZ_LCD_HEIGHT));
    lib.TextXPos += charWidth * lib.TextWidthScale;

    for (uint8_t x = 0; x < charWidth; x++) {
        for (uint8_t u = 0; u < lib.TextWidthScale; u++) {
            uint8_t *fillPtr = fillLinePtr;
            uint8_t b = (1 << 7);
            for (uint8_t y = 0; y < lib.FontHeight; y++) {
                const uint8_t fillColor = *bitImage & b ? lib.Text_FG_Color : lib.Text_BG_Color;
                b >>= 1;
                if (fillColor == lib.Text_TP_Color) {
                    fillPtr += lib.TextHeightScale;
                    continue;
                }

                for (uint8_t v = 0; v < lib.TextHeightScale; v++) {
                    if (
                        fillPtr >= (uint8_t*)RAM_ADDRESS(CurrentBuffer) &&
                        fillPtr < (uint8_t*)RAM_ADDRESS(CurrentBuffer + GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT)
                    ) {
                        *fillPtr = fillColor;
                    }
                    fillPtr++;
                }
            }
            fillLinePtr += GFZ_LCD_HEIGHT;
        }
        bitImage++;
    }
}

//------------------------------------------------------------------------------
// String Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Unclipped Lines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Clipped Lines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Circles
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ellipses
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Triangles
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Unfilled Polygons
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Floodfill
//------------------------------------------------------------------------------

typedef struct FloodFillCord {
    uint32_t x;
    uint8_t y;
} FloodFillCord;

template<>
void GraphY::gfz_FloodFill(uint32_t x, uint8_t y, const uint8_t color) {
    // constexpr size_t FLOODFILL_STACK_SIZE = (3224);
    // going slightly lower to prevent stack overflows (not by much though)
    // constexpr size_t FLOODFILL_STACK_SIZE = (3072);
    // We can use a lot more space on PC
    constexpr size_t FLOODFILL_STACK_SIZE = (8192);
    constexpr size_t Maximum_FloodFill_Stack_Depth = (FLOODFILL_STACK_SIZE / sizeof(FloodFillCord));
    constexpr size_t FloodFill_Safety_Margin = 3;
    if (
        (int32_t)x < lib.ClipXMin || (int32_t)x >= lib.ClipXMax ||
        (int32_t)y < lib.ClipYMin || (int32_t)y >= lib.ClipYMax
    ) {
        return;
    }
    const uint8_t ff_ClipYMin = (uint8_t)lib.ClipYMin;
    const uint8_t ff_ClipYMax = (uint8_t)lib.ClipYMax;
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    uint8_t* pixel = buffer + ((x * GFZ_LCD_HEIGHT) + y);
    const uint8_t replace = *pixel;
    if (replace == color) {
        return; // Same color
    }

    FloodFillCord* const ff_stack = (FloodFillCord*)alloca(FLOODFILL_STACK_SIZE);
    size_t stack_size = 1;
    ff_stack[0] = (FloodFillCord){(uint32_t)x, y};
    int32_t x1;
    bool spanAbove, spanBelow;
    const uint8_t ff_ClipYMaxMinus1 = (uint8_t)(ff_ClipYMax - 1);
    do {
        stack_size--;
        const FloodFillCord ff_cord = ff_stack[stack_size];

        x1 = ff_cord.x;
        uint8_t* src = &buffer[ff_cord.y + x1 * GFZ_LCD_HEIGHT];
        while(x1 >= lib.ClipXMin && *src == replace) {
            x1--;
            src -= GFZ_LCD_HEIGHT;
        }
        x1++;
        src += GFZ_LCD_HEIGHT;
        spanAbove = false;
        spanBelow = false;
        while(
            x1 < lib.ClipXMax &&
            *src == replace &&
            stack_size <= Maximum_FloodFill_Stack_Depth - FloodFill_Safety_Margin
        ) {
            *src = color;
            if (ff_cord.y > ff_ClipYMin) {
                const uint8_t src_pixel = *(src - 1);
                if (!spanAbove && src_pixel == replace) {
                    ff_stack[stack_size] = (FloodFillCord){
                        (uint32_t)x1, (uint8_t)(ff_cord.y - 1)
                    };
                    stack_size++;
                    spanAbove = true;
                } else if (spanAbove && src_pixel != replace) {
                    spanAbove = false;
                }
            }
            if (ff_cord.y < ff_ClipYMaxMinus1) {
                const uint8_t src_pixel = *(src + 1);
                if (!spanBelow && src_pixel == replace) {
                    ff_stack[stack_size] = (FloodFillCord){
                        (uint32_t)x1, (uint8_t)(ff_cord.y + 1)
                    };
                    stack_size++;
                    spanBelow = true;
                } else if (spanBelow && src_pixel != replace) {
                    spanBelow = false;
                }
            }
            x1++;
            src += GFZ_LCD_HEIGHT;
        }
    } while (stack_size > 0);
}

//------------------------------------------------------------------------------
// Sprite Utilities
//------------------------------------------------------------------------------

template<>
gfz_sprite_t *GraphY::gfz_GetSprite(gfz_sprite_t *sprite_buffer, int32_t x, int32_t y) {
    uint8_t* dst_buf = sprite_buffer->data;
    const uint8_t* src_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y + (x * GFZ_LCD_HEIGHT);

    for (uint8_t x_cord = 0; x_cord < sprite_buffer->width; x_cord++) {
        memcpy(dst_buf, src_buf, sprite_buffer->height);
        src_buf += GFZ_LCD_HEIGHT;
        dst_buf += sprite_buffer->height;
    }
    return sprite_buffer;
}

template<>
gfz_sprite_t *GraphY::gfz_GetSpriteChar(char c) {
    const uint8_t *bitImage = lib.TextData + lib.Maximum_Font_Height * (uint32_t)((unsigned char)c);
    gfz_sprite_t * const tempSpriteChar = reinterpret_cast<gfz_sprite_t*>(&lib.TmpCharSprite);
    uint8_t *fillPtr = tempSpriteChar->data;

    const uint8_t CharWidth = gfz_GetCharWidth(c);

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

template<>
gfz_sprite_t *GraphY::gfz_FlipSpriteY(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
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

template<>
gfz_sprite_t *GraphY::gfz_FlipSpriteX(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;
    const uint8_t* src_buf = sprite_in->data + (sprite_in->width - 1);
    uint8_t* dst_buf = sprite_out->data;
    const uint32_t src_jump = (2 * sprite_in->width);
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

template<>
gfz_sprite_t *GraphY::gfz_RotateSpriteC(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
    sprite_out->height = sprite_in->width;
    sprite_out->width = sprite_in->height;
    const uint8_t* src_ptr = (const uint8_t*)&sprite_in->data;
    for (uint8_t y = 0; y < sprite_in->height; y++) {
        for (uint8_t x = 0; x < sprite_in->width; x++) {
            sprite_out->data[y + (sprite_in->width - x - 1) * sprite_in->height] = *src_ptr;
            src_ptr++;
        }
    }
    return sprite_out;

}

template<>
gfz_sprite_t *GraphY::gfz_RotateSpriteCC(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
    sprite_out->height = sprite_in->width;
    sprite_out->width = sprite_in->height;
    const uint8_t* src_ptr = (const uint8_t*)&sprite_in->data;
    for (uint8_t y = 0; y < sprite_in->height; y++) {
        for (uint8_t x = 0; x < sprite_in->width; x++) {
            sprite_out->data[(sprite_in->height - y - 1) + x * sprite_in->height] = *src_ptr;
            src_ptr++;
        }
    }
    return sprite_out;
}

//------------------------------------------------------------------------------
// Sprite Drawing
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_Sprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y) {
    gfz_Wait();
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y + (x * GFZ_LCD_HEIGHT);

    for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
        memcpy(dst_buf, src_buf, sprite->height);
        src_buf += sprite->height;
        dst_buf += GFZ_LCD_HEIGHT;
    }
}

template<>
void GraphY::gfz_Sprite(const gfz_sprite_t * sprite, int32_t x, int32_t y) {
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
        gfz_Sprite_NoClip(sprite, (uint32_t)x, (uint8_t)y);
        return;
    }
    gfz_Wait();
    const uint8_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint8_t min_clipY = (y < lib.ClipYMin) ? (lib.ClipYMin - y) : 0;
    const uint8_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;
    const uint8_t max_clipY = ((y + sprite->height) > lib.ClipYMax) ? ((y + sprite->height) - lib.ClipYMax) : 0;

    uint8_t sizeX = sprite->width - (min_clipX + max_clipX);
    uint8_t sizeY = sprite->height - (min_clipY + max_clipY);

    const uint8_t* src_buf = sprite->data + min_clipY + (min_clipX * sprite->height);

    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (y + min_clipY) + ((x + min_clipX) * GFZ_LCD_HEIGHT);

    for (uint8_t x_cord = 0; x_cord < sizeX; x_cord++) {
        memcpy(dst_buf, src_buf, sizeY);
        src_buf += sprite->height;
        dst_buf += GFZ_LCD_HEIGHT;
    }
}

template<>
void GraphY::gfz_TransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y) {
    gfz_Wait();
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y + (x * GFZ_LCD_HEIGHT);
    const uint32_t dst_jump = GFZ_LCD_HEIGHT - sprite->height;

    for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
        for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
            if (*src_buf != lib.Transparent_Color) {
                *dst_buf = *src_buf;
            }
            src_buf++;
            dst_buf++;
        }
        dst_buf += dst_jump;
    }
}

template<>
void GraphY::gfz_TransparentSprite(const gfz_sprite_t * sprite, int32_t x, int32_t y) {
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
        gfz_TransparentSprite_NoClip(sprite, (uint32_t)x, (uint8_t)y);
        return;
    }
    gfz_Wait();
    const uint8_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint8_t min_clipY = (y < lib.ClipYMin) ? (lib.ClipYMin - y) : 0;
    const uint8_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;
    const uint8_t max_clipY = ((y + sprite->height) > lib.ClipYMax) ? ((y + sprite->height) - lib.ClipYMax) : 0;

    uint8_t sizeX = sprite->width - (min_clipX + max_clipX);
    uint8_t sizeY = sprite->height - (min_clipY + max_clipY);
    const uint8_t* src_buf = sprite->data + min_clipY + (min_clipX * sprite->height);
    const uint8_t clip_jumpY = min_clipY + max_clipY;

    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (y + min_clipY) + ((x + min_clipX) * GFZ_LCD_HEIGHT);
    const uint32_t dst_jump = GFZ_LCD_HEIGHT - sizeY;

    for (uint8_t x_cord = 0; x_cord < sizeX; x_cord++) {
        for (uint8_t y_cord = 0; y_cord < sizeY; y_cord++) {
            if (*src_buf != lib.Transparent_Color) {
                *dst_buf = *src_buf;
            }
            src_buf++;
            dst_buf++;
        }
        src_buf += clip_jumpY;
        dst_buf += dst_jump;
    }
}

//------------------------------------------------------------------------------
// Scaled Sprites
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_ScaledSprite_NoClip(
    const gfz_sprite_t *sprite,
    const uint32_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) {
    if (width_scale == 0 || height_scale == 0) {
        return;
    }
    gfz_Wait();
    const uint8_t* src_buf = sprite->data;
    uint8_t* buf_start = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y + (x * GFZ_LCD_HEIGHT);
    uint8_t* dst_buf = buf_start;
    const uint32_t dst_jump = (GFZ_LCD_HEIGHT * width_scale) - (sprite->height * height_scale);

    for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
        for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
            const uint8_t color = *src_buf++;
            for (uint8_t v = 0; v < height_scale; v++) {
                *dst_buf++ = color;
            }
        }
        dst_buf += dst_jump;
    }

    if (width_scale == 1) {
        return;
    }

    // memcpy columns
    uint8_t* dst_col = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y + (x * GFZ_LCD_HEIGHT);
    const uint32_t copySize = sprite->height * height_scale; // This could probably be uint8_t
    for (uint8_t col = 0; col < sprite->width; col++) {
        const uint8_t* const src_col = (uint8_t*)dst_col;
        dst_col += GFZ_LCD_HEIGHT;
        for (uint8_t x_cord = 1; x_cord < width_scale; x_cord++) {
            memcpy(dst_col, src_col, copySize);
            dst_col += GFZ_LCD_HEIGHT;
        }
    }
}

/* gfz_ScaledTransparentSprite_NoClip */

template<>
void GraphY::gfz_ScaledTransparentSprite_NoClip(
    const gfz_sprite_t *sprite,
    const uint32_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) {
    if (width_scale == 0 || height_scale == 0) {
        return;
    }
    gfz_Wait();
    const uint8_t* src_buf = sprite->data;
    uint8_t* buf_start = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + y + (x * GFZ_LCD_HEIGHT);
    uint8_t* dst_buf = buf_start;
    const uint32_t dst_jump = GFZ_LCD_HEIGHT - (sprite->height * height_scale);

    for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
        for (uint8_t u = 0; u < width_scale; u++) {
            for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
                const uint8_t color = *src_buf++;
                if (color == lib.Transparent_Color) {
                    dst_buf += height_scale;
                    continue;
                }
                for (uint8_t v = 0; v < height_scale; v++) {
                    *dst_buf++ = color;
                }
            }
            dst_buf += dst_jump;
            src_buf -= sprite->height;
        }
        src_buf += sprite->height;
    }
}

template<>
gfz_sprite_t* GraphY::gfz_ScaleSprite(
    const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out
) {
    uint8_t* dst_ptr = sprite_out->data;
    for (uint8_t x = 0; x < sprite_out->width; x++) {
        const uint8_t* src_ptr = sprite_in->data + ((x * sprite_in->width) / sprite_in->width) * sprite_out->height;
        for (uint8_t y = 0; y < sprite_out->height; y++) {
            *dst_ptr = *(src_ptr + (y * sprite_out->height) / sprite_in->height);
            dst_ptr++;
        }
    }
    return sprite_out;
}

//------------------------------------------------------------------------------
// Sprite Rotation
//------------------------------------------------------------------------------

#include <math.h>

template<>
gfz_sprite_t *GraphY::gfz_RotateScaleSprite(
    const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out,
    uint8_t angle, uint8_t scale
) {
    const uint8_t in_size = sprite_in->width;
    uint32_t temp_size = sprite_in->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;
    sprite_out->width = out_size;
    sprite_out->height = out_size;
    memset(sprite_out->data, lib.Transparent_Color, out_size * out_size);

    // const int32_t sin_val = gfz_Sin(angle) * 128 / scale;
    // const int32_t cos_val = gfz_Cos(angle) * 128 / scale;

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
            int32_t x_write = (int32_t)x_pos;
            int32_t y_write = (int32_t)y_pos;
            if (
                x_write < in_size && y_write < in_size &&
                x_write >= 0 && y_write >= 0
            ) {
                *dst_ptr = sprite_in->data[x_write * in_size + y_write];
            }
            dst_ptr++;
        }
        // printf("\n");
    }
    // printf("\n");
    return sprite_out;
}

template<>
uint8_t GraphY::gfz_RotatedScaledSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x0, uint8_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * 0.0245436926f;

    const float cos_f = (float)cosf(angle_f);
    const float sin_f = (float)sinf(angle_f);

    const float in_size_f = (float)in_size;
    const float out_size_f = (float)out_size;

    const float size_ratio = in_size_f / out_size_f;

    const float cos_jump = cos_f * size_ratio;
    const float sin_jump = sin_f * size_ratio;

    const float in_size_mult = -0.5f * in_size_f;
    float yc_cos = in_size_mult * (cos_f + sin_f - 1.0f);
    float yc_sin = in_size_mult * (cos_f - sin_f - 1.0f);

    uint8_t *__restrict plot = buffer + x0 * GFZ_LCD_HEIGHT + y0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t y_pos = (uint8_t)(int)xc_cos;
            uint8_t x_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                uint8_t color = sprite->data[y_pos + x_pos * in_size];
                *plot = color;
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_HEIGHT - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

template<>
uint8_t GraphY::gfz_RotatedScaledTransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x0, uint8_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * 0.0245436926f;

    const float cos_f = (float)cosf(angle_f);
    const float sin_f = (float)sinf(angle_f);

    const float in_size_f = (float)in_size;
    const float out_size_f = (float)out_size;

    const float size_ratio = in_size_f / out_size_f;

    const float cos_jump = cos_f * size_ratio;
    const float sin_jump = sin_f * size_ratio;

    const float in_size_mult = -0.5f * in_size_f;
    float yc_cos = in_size_mult * (cos_f + sin_f - 1.0f);
    float yc_sin = in_size_mult * (cos_f - sin_f - 1.0f);

    uint8_t *__restrict plot = buffer + x0 * GFZ_LCD_HEIGHT + y0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t y_pos = (uint8_t)(int)xc_cos;
            uint8_t x_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                uint8_t color = sprite->data[y_pos + x_pos * in_size];
                if (color != lib.Transparent_Color) {
                    *plot = color;
                }
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_HEIGHT - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

template<>
uint8_t GraphY::gfz_RotatedScaledSprite(const gfz_sprite_t *sprite, int32_t x0, int32_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * 0.0245436926f;

    const float cos_f = (float)cosf(angle_f);
    const float sin_f = (float)sinf(angle_f);

    const float in_size_f = (float)in_size;
    const float out_size_f = (float)out_size;

    const float size_ratio = in_size_f / out_size_f;

    const float cos_jump = cos_f * size_ratio;
    const float sin_jump = sin_f * size_ratio;

    const float in_size_mult = -0.5f * in_size_f;
    float yc_cos = in_size_mult * (cos_f + sin_f - 1.0f);
    float yc_sin = in_size_mult * (cos_f - sin_f - 1.0f);

    uint8_t *__restrict plot = buffer + x0 * GFZ_LCD_HEIGHT + y0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t y_pos = (uint8_t)(int)xc_cos;
            uint8_t x_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                if (
                    (int32_t)(x + x0) >= lib.ClipXMin &&
                    (int32_t)(x + x0) < lib.ClipXMax &&
                    (int32_t)(y + y0) >= lib.ClipYMin &&
                    (int32_t)(y + y0) < lib.ClipYMax
                ) {
                    uint8_t color = sprite->data[y_pos + x_pos * in_size];
                    *plot = color;
                }
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_HEIGHT - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

template<>
uint8_t GraphY::gfz_RotatedScaledTransparentSprite(const gfz_sprite_t *sprite, int32_t x0, int32_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * 0.0245436926f;

    const float cos_f = (float)cosf(angle_f);
    const float sin_f = (float)sinf(angle_f);

    const float in_size_f = (float)in_size;
    const float out_size_f = (float)out_size;

    const float size_ratio = in_size_f / out_size_f;

    const float cos_jump = cos_f * size_ratio;
    const float sin_jump = sin_f * size_ratio;

    const float in_size_mult = -0.5f * in_size_f;
    float yc_cos = in_size_mult * (cos_f + sin_f - 1.0f);
    float yc_sin = in_size_mult * (cos_f - sin_f - 1.0f);

    uint8_t *__restrict plot = buffer + x0 * GFZ_LCD_HEIGHT + y0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t y_pos = (uint8_t)(int)xc_cos;
            uint8_t x_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                if (
                    (int32_t)(x + x0) >= lib.ClipXMin &&
                    (int32_t)(x + x0) < lib.ClipXMax &&
                    (int32_t)(y + y0) >= lib.ClipYMin &&
                    (int32_t)(y + y0) < lib.ClipYMax
                ) {
                    uint8_t color = sprite->data[y_pos + x_pos * in_size];
                    if (color != lib.Transparent_Color) {
                        *plot = color;
                    }
                }
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_HEIGHT - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

//------------------------------------------------------------------------------
// Tilemaps
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// RLET Sprites
//------------------------------------------------------------------------------

template<>
void GraphY::gfz_RLETSprite_NoClip(const gfz_rletsprite_t *sprite, const uint32_t x, const uint8_t y) {
    gfz_Wait();
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (y + (GFZ_LCD_HEIGHT * x));
    const uint32_t dst_jump = GFZ_LCD_HEIGHT - sprite->height;

    for (uint8_t posX = 0; posX < sprite->width; posX++) {
        uint32_t posY = 0;
        while (posY < sprite->height) {
            const uint8_t jump_TP = *src_buf++;
            posY += jump_TP;
            dst_buf += jump_TP;

            if (posY >= sprite->height) {
                break;
            }

            const uint8_t len = *src_buf++;
            posY += len;
            for (uint8_t r = 0; r < len; r++) {
                *dst_buf++ = *src_buf++;
            }
        }
        dst_buf += dst_jump;
    }
}

template<>
void GraphY::gfz_RLETSprite(const gfz_rletsprite_t *__restrict sprite, const int32_t x, const int32_t y) {
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
        // If fully inside the clipping region
        gfz_RLETSprite_NoClip(sprite, (uint32_t)x, (uint8_t)y);
        return;
    }
    gfz_Wait();

    // X axis (Columns) are properly clipped. The Y axis (Rows) uses naive clipping

    const uint32_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint32_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;

    const uint32_t sizeX = sprite->width - (min_clipX + max_clipX);

    const uint8_t *__restrict src_buf = sprite->data;
    uint8_t *__restrict dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (y + (GFZ_LCD_HEIGHT * (x + min_clipX)));
    const uint32_t dst_jump = GFZ_LCD_HEIGHT - sprite->height;

    // Fast forward through the decompression if needed
    for (uint32_t posX = 0; posX < min_clipX; posX++) {
        uint32_t posY = 0;
        while (posY < sprite->height) {
            posY += *src_buf++;

            if (posY >= sprite->height) {
                break;
            }

            const uint8_t len = *src_buf++;
            src_buf += len;
            posY += len;
        }
    }

    const uint8_t sprite_height = sprite->height;
    const uint8_t maxY = lib.ClipYMax;
    const uint8_t minY = lib.ClipYMin;

    for (uint32_t posX = 0; posX < sizeX; posX++) {
        uint32_t posY = 0;
        while (posY < sprite_height) {
            const uint8_t jump_TP = *src_buf++;
            posY += jump_TP;
            dst_buf += jump_TP;

            if (posY >= sprite_height) {
                break;
            }

            const uint8_t len = *src_buf++;

            // we can use a fast memcpy if no clipping is needed
            if (y + posY >= minY && y + posY + len <= maxY) {
                memcpy(dst_buf, src_buf, len);
                dst_buf += len;
                src_buf += len;
                posY += len;
                continue;
            }

            for (uint8_t r = 0; r < len; r++) {
                if (
                    y + posY >= minY &&
                    y + posY < maxY
                ) {
                    *dst_buf = *src_buf;
                }
                src_buf++;
                posY++;
                dst_buf++;
            }
        }
        dst_buf += dst_jump;
    }
}

template<>
gfz_sprite_t *GraphY::gfz_ConvertFromRLETSprite(const gfz_rletsprite_t *sprite_in, gfz_sprite_t *sprite_out) {
    uint8_t IYL = sprite_in->width;
    uint8_t IYH = sprite_in->height;
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;

    const uint8_t *HL = sprite_in->data;
    uint8_t* DE = sprite_out->data;
    uint8_t A, B, C;

ConvertFromRLETSprite_Row:
    A = IYH;

ConvertFromRLETSprite_Trans:
    B = *HL;
    ++HL;
    if (B == 0) {
        goto ConvertFromRLETSprite_Opaque;
    }

// ConvertFromRLETSprite_TransLoop:
    memset(DE, lib.Transparent_Color, B);

    DE += B;
    A -= B;
    if (A == 0) {
        goto ConvertFromRLETSprite_RowEnd;
    }

ConvertFromRLETSprite_Opaque:
    C = *HL;
    ++HL;
    memcpy(DE, HL, C);
    DE += C;
    HL += C;
    A -= C;
    if (A != 0) {
        goto ConvertFromRLETSprite_Trans;
    }

ConvertFromRLETSprite_RowEnd:
    --IYL;
    if (IYL != 0) {
        goto ConvertFromRLETSprite_Row;
    }
    return sprite_out;
}


template<>
gfz_rletsprite_t *GraphY::gfz_ConvertToRLETSprite(const gfz_sprite_t *sprite_in, gfz_rletsprite_t *sprite_out) {
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;

    const uint8_t* src_ptr = sprite_in->data;
    uint8_t* dst_ptr = sprite_out->data;

    for (uint8_t x = 0; x < sprite_in->width; x++) {
        uint8_t y = 0;

        while (y < sprite_in->height) {
            uint8_t transparent_run_length = 0;
            while (y < sprite_in->height && *src_ptr == lib.Transparent_Color) {
                transparent_run_length++;
                y++;
                src_ptr++;
            }
            *dst_ptr = transparent_run_length;
            dst_ptr++;

            if (y >= sprite_in->height) {
                break;
            }

            uint8_t* const opaque_element = dst_ptr;
            dst_ptr++;
            uint8_t opaque_run_length = 0;
            while (y < sprite_in->height && *src_ptr != lib.Transparent_Color) {
                opaque_run_length++;
                y++;
                *dst_ptr = *src_ptr;
                src_ptr++;
                dst_ptr++;
            }
            *opaque_element = opaque_run_length;
        }
    }
    return sprite_out;
}

template<>
gfz_rletsprite_t *GraphY::gfz_ConvertToNewRLETSprite(const gfz_sprite_t *sprite_in, void *(*malloc_routine)(size_t)) {
    size_t rlet_size = sizeof(gfz_sprite_t);
    const uint8_t* src_ptr = sprite_in->data;

    // Calculates rlet_size
    for (uint8_t x = 0; x < sprite_in->width; x++) {
        uint8_t y = 0;

        while (y < sprite_in->height) {
            while (y < sprite_in->height && *src_ptr == lib.Transparent_Color) {
                y++;
                src_ptr++;
            }
            rlet_size++;

            if (y >= sprite_in->height) {
                break;
            }

            rlet_size++;
            while (y < sprite_in->height && *src_ptr != lib.Transparent_Color) {
                y++;
                src_ptr++;
                rlet_size++;
            }
        }
    }

    gfz_rletsprite_t* sprite_out = reinterpret_cast<gfz_rletsprite_t*>((*malloc_routine)(rlet_size));
    if (sprite_out == nullptr) {
        return nullptr; // Allocation failure
    }
    return gfz_ConvertToRLETSprite(sprite_in, sprite_out);
}

//------------------------------------------------------------------------------
// Wrapper Functions
//------------------------------------------------------------------------------

#include "lib_graphy.hpp"
