
#include <PortCE.h>

#include <graphx.h>

#include "lib_graphz.hpp"
#include "graphx_data.h"

struct GraphX_Type {};

typedef GraphZ<GraphX_Type> GraphX;

static GraphX lib("graphx", graphx_DefaultCharSpacing, graphx_DefaultTextData);

//------------------------------------------------------------------------------
// Rendering Primitives
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_Begin(void) {
    util_Begin(*this);
    lcd_VideoMode = lcd_BGR8bit;
}

template<>
void GraphX::gfz_End(void) {
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
void GraphX::gfz_BlitRows(gfz_location_t src, uint8_t y_loc, uint8_t num_rows) {
    const uint8_t *src_buf = gfz_vram;
    uint8_t *dst_buf = gfz_vram + (GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
    if (src) {
        src_buf = gfz_vram + (GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
        dst_buf = gfz_vram;
    }
    src_buf += y_loc * GFZ_LCD_WIDTH;
    dst_buf += y_loc * GFZ_LCD_WIDTH;
    memcpy(dst_buf, src_buf, num_rows * GFZ_LCD_WIDTH);
}

template<>
void GraphX::gfz_BlitRectangle(
    gfz_location_t src,
    uint32_t x,
    uint8_t y,
    uint32_t width,
    uint32_t height
) {
    const uint8_t *src_buf = gfz_vram;
    uint8_t *dst_buf = gfz_vram + (GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
    if (src) {
        src_buf = gfz_vram + (GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT);
        dst_buf = gfz_vram;
    }
    src_buf += y + (x * GFZ_LCD_WIDTH);
    dst_buf += y + (x * GFZ_LCD_WIDTH);
    for (uint32_t y_cord = 0; y_cord < height; y_cord++) {
        memcpy(dst_buf, src_buf, width);
        src_buf += GFZ_LCD_WIDTH;
        dst_buf += GFZ_LCD_WIDTH;
    }
}

template<>
void GraphX::gfz_CopyRectangle(
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
    src_buf += src_x + (src_y * GFZ_LCD_WIDTH);
    dst_buf += dst_x + (dst_y * GFZ_LCD_WIDTH);
    const uint32_t jump = GFZ_LCD_WIDTH - width;
    if (buf_overlap == true) {
        for (uint32_t y_cord = 0; y_cord < height; y_cord++) {
            memmove(dst_buf, src_buf, width);
            src_buf += jump;
            dst_buf += jump;
        }
        return;
    }
    for (uint32_t y_cord = 0; y_cord < height; y_cord++) {
        memcpy(dst_buf, src_buf, width);
        src_buf += jump;
        dst_buf += jump;
    }
}

//------------------------------------------------------------------------------
// Screen Shifting
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_ShiftDown(uint8_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + ((lib.ClipYMin + (int8_t)pixels) * GFX_LCD_WIDTH);
    const size_t copySize = lib.ClipXMax - lib.ClipXMin;
    int24_t y0 = lib.ClipYMin + (int8_t)pixels;
    int24_t y1 = lib.ClipYMax;
    src_buf += pixels * GFX_LCD_WIDTH;
    dst_buf += pixels * GFX_LCD_WIDTH;
    for (int24_t y = y0; y < y1; y++) {
        memcpy(dst_buf, src_buf, copySize);
        src_buf -= GFX_LCD_WIDTH;
        dst_buf -= GFX_LCD_WIDTH;
    }
}

template<>
void GraphX::gfz_ShiftUp(uint8_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + ((lib.ClipYMin - (int8_t)pixels) * GFX_LCD_WIDTH);
    const int24_t copySize = lib.ClipXMax - lib.ClipXMin - (int24_t)pixels;
    if (copySize <= 0) { return; }
    int24_t y0 = lib.ClipYMin + (int8_t)pixels;
    int24_t y1 = lib.ClipYMax;
    src_buf -= pixels * GFX_LCD_WIDTH;
    dst_buf -= pixels * GFX_LCD_WIDTH;
    for (int24_t y = y0; y < y1; y++) {
        memcpy(dst_buf, src_buf, (size_t)copySize);
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

template<>
void GraphX::gfz_ShiftLeft(uint32_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.ClipXMin - (int24_t)pixels) + (lib.ClipYMin * GFX_LCD_WIDTH);
    const int24_t copySize = lib.ClipXMax - lib.ClipXMin - (int24_t)pixels;
    if (copySize <= 0) { return; }
    int24_t y0 = lib.ClipYMin;
    int24_t y1 = lib.ClipYMax;
    for (int24_t y = y0; y < y1; y++) {
        memmove(dst_buf, src_buf, (size_t)copySize); // memcpy would be UB
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

template<>
void GraphX::gfz_ShiftRight(uint32_t pixels) {
    if (pixels == 0) { return; }
    const uint8_t* src_buf = (const uint8_t*)RAM_ADDRESS(CurrentBuffer) + lib.ClipXMin + (lib.ClipYMin * GFX_LCD_WIDTH);
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.ClipXMin + (int24_t)pixels) + (lib.ClipYMin * GFX_LCD_WIDTH);
    const size_t copySize = lib.ClipXMax - lib.ClipXMin - pixels;
    int24_t y0 = lib.ClipYMin;
    int24_t y1 = lib.ClipYMax;
    for (int24_t y = y0; y < y1; y++) {
        memmove(dst_buf, src_buf, copySize); // memcpy would be UB
        src_buf += GFX_LCD_WIDTH;
        dst_buf += GFX_LCD_WIDTH;
    }
}

//------------------------------------------------------------------------------
// Pixel Functions
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_SetPixel_NoClip(uint32_t x, uint8_t y, uint8_t color) {
    if (x < GFZ_LCD_WIDTH && y < GFZ_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint32_t)x + (y * GFZ_LCD_WIDTH)] = color;
    }
}

template<>
void GraphX::gfz_SetPixel(uint32_t x, uint8_t y) {
    if (x < GFZ_LCD_WIDTH && y < GFZ_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint32_t)x + (y * GFZ_LCD_WIDTH)] = lib.Color;
    }
}

template<>
uint8_t GraphX::gfz_GetPixel(uint32_t x, uint8_t y) {
    return ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(x & 0xFFFF) + ((uint32_t)y * GFZ_LCD_WIDTH)];
}

//------------------------------------------------------------------------------
// Screen Filling
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Horizontal Lines
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_HorizLine_NoClip(uint32_t x, uint8_t y, uint32_t length) {
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint32_t)y * GFZ_LCD_WIDTH));
    memset(fill, lib.Color, length);
}

//------------------------------------------------------------------------------
// Vertical Lines
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_VertLine_NoClip(uint32_t x, uint8_t y, uint32_t length) {
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint32_t)y * GFZ_LCD_WIDTH));
    for (uint32_t y_cord = 0; y_cord < length; y_cord++) {
        *fill = lib.Color;
        fill += GFZ_LCD_WIDTH;
    }
}

//------------------------------------------------------------------------------
// Filled Rectangle
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_FillRectangle_NoClip(uint32_t x, uint8_t y, uint32_t width, uint8_t height) {
    if (width == 0 || height == 0) {
        return;
    }
    uint8_t *fill = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + ((uint32_t)y * GFZ_LCD_WIDTH));
    for (uint8_t y_cord = 0; y_cord < height; y_cord++) {
        memset(fill, lib.Color, width);
        fill += GFZ_LCD_WIDTH;
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
    uint8_t *fillLinePtr = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.TextXPos + (lib.TextYPos * GFZ_LCD_WIDTH));

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
            fillLinePtr += GFZ_LCD_WIDTH;
        }
        bitImage++;
    }
}

template<>
void GraphX::gfz_PrintChar(const char c) {
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
    uint8_t *fillLinePtr = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (lib.TextXPos + (lib.TextYPos * GFZ_LCD_WIDTH));
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
                        fillPtr < (uint8_t*)RAM_ADDRESS(CurrentBuffer + GFZ_LCD_WIDTH * GFZ_LCD_HEIGHT)
                    ) {
                        *fillPtr = fillColor;
                    }
                    fillPtr++;
                }
            }
            fillLinePtr += GFZ_LCD_WIDTH;
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
void GraphX::gfz_FloodFill(uint32_t x, uint8_t y, const uint8_t color) {
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
    uint8_t* pixel = buffer + ((y * GFZ_LCD_WIDTH) + x);
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
        uint8_t* src = &buffer[ff_cord.y * GFZ_LCD_WIDTH + x1];
        while(x1 >= lib.ClipXMin && *src == replace) {
            x1--;
            src--;
        }
        x1++;
        src++;
        spanAbove = false;
        spanBelow = false;
        while(
            x1 < lib.ClipXMax &&
            *src == replace &&
            stack_size <= Maximum_FloodFill_Stack_Depth - FloodFill_Safety_Margin
        ) {
            *src = color;
            if (ff_cord.y > ff_ClipYMin) {
                const uint8_t src_pixel = *(src - GFZ_LCD_WIDTH);
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
                const uint8_t src_pixel = *(src + GFZ_LCD_WIDTH);
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
            src++;
        }
    } while (stack_size > 0);
}

//------------------------------------------------------------------------------
// Sprite Utilities
//------------------------------------------------------------------------------

template<>
gfz_sprite_t *GraphX::gfz_GetSprite(gfz_sprite_t *sprite_buffer, int32_t x, int32_t y) {
    uint8_t* dst_buf = sprite_buffer->data;
    const uint8_t* src_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFZ_LCD_WIDTH);

    for (uint8_t y_cord = 0; y_cord < sprite_buffer->height; y_cord++) {
        memcpy(dst_buf, src_buf, sprite_buffer->width);
        src_buf += GFZ_LCD_WIDTH;
        dst_buf += sprite_buffer->width;
    }
    return sprite_buffer;
}

template<>
gfz_sprite_t *GraphX::gfz_GetSpriteChar(char c) {
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
gfz_sprite_t *GraphX::gfz_FlipSpriteY(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
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
gfz_sprite_t *GraphX::gfz_FlipSpriteX(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
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
gfz_sprite_t *GraphX::gfz_RotateSpriteC(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
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

template<>
gfz_sprite_t *GraphX::gfz_RotateSpriteCC(const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out) {
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

//------------------------------------------------------------------------------
// Sprite Drawing
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_Sprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y) {
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFZ_LCD_WIDTH);

    for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
        memcpy(dst_buf, src_buf, sprite->width);
        src_buf += sprite->width;
        dst_buf += GFZ_LCD_WIDTH;
    }
}

template<>
void GraphX::gfz_Sprite(const gfz_sprite_t *sprite, int32_t x, int32_t y) {
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
    const uint8_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint8_t min_clipY = (y < lib.ClipYMin) ? (lib.ClipYMin - y) : 0;
    const uint8_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;
    const uint8_t max_clipY = ((y + sprite->height) > lib.ClipYMax) ? ((y + sprite->height) - lib.ClipYMax) : 0;

    uint8_t sizeX = sprite->width - (min_clipX + max_clipX);
    uint8_t sizeY = sprite->height - (min_clipY + max_clipY);

    const uint8_t* src_buf = sprite->data + min_clipX + (min_clipY * sprite->width);

    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + min_clipX) + ((y + min_clipY) * GFZ_LCD_WIDTH);

    for (uint8_t y_cord = 0; y_cord < sizeY; y_cord++) {
        memcpy(dst_buf, src_buf, sizeX);
        src_buf += sprite->width;
        dst_buf += GFZ_LCD_WIDTH;
    }
}

template<>
void GraphX::gfz_TransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x, uint8_t y) {
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFZ_LCD_WIDTH);
    const uint32_t dst_jump = GFZ_LCD_WIDTH - sprite->width;

    for (uint8_t y_cord = 0; y_cord < sprite->height; y_cord++) {
        for (uint8_t x_cord = 0; x_cord < sprite->width; x_cord++) {
            *dst_buf = (*src_buf != lib.Transparent_Color) ? *src_buf : *dst_buf;
            src_buf++;
            dst_buf++;
        }
        dst_buf += dst_jump;
    }
}

template<>
void GraphX::gfz_TransparentSprite(const gfz_sprite_t *sprite, int32_t x, int32_t y) {
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
    const uint8_t min_clipX = (x < lib.ClipXMin) ? (lib.ClipXMin - x) : 0;
    const uint8_t min_clipY = (y < lib.ClipYMin) ? (lib.ClipYMin - y) : 0;
    const uint8_t max_clipX = ((x + sprite->width) > lib.ClipXMax) ? ((x + sprite->width) - lib.ClipXMax) : 0;
    const uint8_t max_clipY = ((y + sprite->height) > lib.ClipYMax) ? ((y + sprite->height) - lib.ClipYMax) : 0;

    uint8_t sizeX = sprite->width - (min_clipX + max_clipX);
    uint8_t sizeY = sprite->height - (min_clipY + max_clipY);

    const uint8_t* src_buf = sprite->data + min_clipX + (min_clipY * sprite->width);
    const uint8_t clip_jumpX = min_clipX + max_clipX;

    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + min_clipX) + ((y + min_clipY) * GFZ_LCD_WIDTH);
    const uint32_t dst_jump = GFZ_LCD_WIDTH - sizeX;

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

//------------------------------------------------------------------------------
// Scaled Sprites
//------------------------------------------------------------------------------

template<>
void GraphX::gfz_ScaledSprite_NoClip(
    const gfz_sprite_t *sprite,
    const uint32_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) {
    if (width_scale == 0 || height_scale == 0) {
        return;
    }
    const uint8_t* src_buf = sprite->data;
    uint8_t* buf_start = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFZ_LCD_WIDTH);
    uint8_t* dst_buf = buf_start;
    const uint32_t dst_jump = (GFZ_LCD_WIDTH * height_scale) - (sprite->width * width_scale);

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
    uint8_t* dst_row = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFZ_LCD_WIDTH);
    const uint32_t copySize = sprite->width * width_scale; // This could probably be uint8_t
    for (uint8_t row = 0; row < sprite->height; row++) {
        const uint8_t* const src_row = (uint8_t*)dst_row;
        dst_row += GFZ_LCD_WIDTH;
        for (uint8_t y_cord = 1; y_cord < height_scale; y_cord++) {
            memcpy(dst_row, src_row, copySize);
            dst_row += GFZ_LCD_WIDTH;
        }
    }
}

/* gfz_ScaledTransparentSprite_NoClip */

template<>
void GraphX::gfz_ScaledTransparentSprite_NoClip(
    const gfz_sprite_t *sprite,
    const uint32_t x,
    const uint8_t y,
    const uint8_t width_scale,
    const uint8_t height_scale
) {
    if (width_scale == 0 || height_scale == 0) {
        return;
    }
    const uint8_t* src_buf = sprite->data;
    uint8_t* buf_start = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + x + (y * GFZ_LCD_WIDTH);
    uint8_t* dst_buf = buf_start;
    const uint32_t dst_jump = GFZ_LCD_WIDTH - (sprite->width * width_scale);

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

template<>
gfz_sprite_t* GraphX::gfz_ScaleSprite(
    const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out
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

/* gfz_RotateScaleSprite */

#include <math.h>

template<>
gfz_sprite_t *GraphX::gfz_RotateScaleSprite(
    const gfz_sprite_t *sprite_in, gfz_sprite_t *sprite_out,
    uint8_t angle, uint8_t scale
) {
    const uint8_t in_size = sprite_in->width;
    uint32_t temp_size = sprite_in->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;
    sprite_out->width = out_size;
    sprite_out->height = out_size;
    memset(sprite_out->data, lib.Transparent_Color, out_size * out_size);

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
            x_pos *= (float)in_size;
            y_pos *= (float)in_size;
            int32_t x_write = (int32_t)x_pos;
            int32_t y_write = (int32_t)y_pos;
            if (
                x_write < in_size && y_write < in_size &&
                x_write >= 0 && y_write >= 0
            ) {
                *dst_ptr = sprite_in->data[x_write + y_write * in_size];
            }
            dst_ptr++;
        }
    }
    return sprite_out;
}

template<>
uint8_t GraphX::gfz_RotatedScaledSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x0, uint8_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * -0.0245436926f;

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

    uint8_t *__restrict plot = buffer + y0 * GFZ_LCD_WIDTH + x0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t x_pos = (uint8_t)(int)xc_cos;
            uint8_t y_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                uint8_t color = sprite->data[y_pos * in_size + x_pos];
                *plot = color;
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_WIDTH - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

template<>
uint8_t GraphX::gfz_RotatedScaledTransparentSprite_NoClip(const gfz_sprite_t *sprite, uint32_t x0, uint8_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * -0.0245436926f;

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

    uint8_t *__restrict plot = buffer + y0 * GFZ_LCD_WIDTH + x0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t x_pos = (uint8_t)(int)xc_cos;
            uint8_t y_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                uint8_t color = sprite->data[y_pos * in_size + x_pos];
                if (color != lib.Transparent_Color) {
                    *plot = color;
                }
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_WIDTH - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

template<>
uint8_t GraphX::gfz_RotatedScaledSprite(const gfz_sprite_t *sprite, int32_t x0, int32_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * -0.0245436926f;

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

    uint8_t *__restrict plot = buffer + y0 * GFZ_LCD_WIDTH + x0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t x_pos = (uint8_t)(int)xc_cos;
            uint8_t y_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                if (
                    (int32_t)(y + x0) >= lib.ClipXMin &&
                    (int32_t)(y + x0) < lib.ClipXMax &&
                    (int32_t)(x + y0) >= lib.ClipYMin &&
                    (int32_t)(x + y0) < lib.ClipYMax
                ) {
                    uint8_t color = sprite->data[y_pos * in_size + x_pos];
                    *plot = color;
                }
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_WIDTH - out_size;
        yc_cos += cos_jump;
        yc_sin -= sin_jump;
    }
    return out_size;
}

template<>
uint8_t GraphX::gfz_RotatedScaledTransparentSprite(const gfz_sprite_t *sprite, int32_t x0, int32_t y0, uint8_t angle, uint8_t scale) {
    gfz_Wait();
    uint8_t * const buffer = (uint8_t*)RAM_ADDRESS(CurrentBuffer);
    const uint8_t in_size = sprite->width;
    uint32_t temp_size = sprite->width * scale / 64;
    const uint8_t out_size = (temp_size >= 256) ? 255 : temp_size;

    const float angle_f = (float)angle * -0.0245436926f;

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

    uint8_t *__restrict plot = buffer + y0 * GFZ_LCD_WIDTH + x0;
    for (uint8_t x = 0; x < out_size; x++) {
        float xc_cos = yc_sin;
        float xc_sin = yc_cos;
        for (uint8_t y = 0; y < out_size; y++) {
            uint8_t x_pos = (uint8_t)(int)xc_cos;
            uint8_t y_pos = (uint8_t)(int)xc_sin;
            if (
                x_pos < in_size && y_pos < in_size &&
                x_pos >= 0 && y_pos >= 0
            ) {
                if (
                    (int32_t)(y + x0) >= lib.ClipXMin &&
                    (int32_t)(y + x0) < lib.ClipXMax &&
                    (int32_t)(x + y0) >= lib.ClipYMin &&
                    (int32_t)(x + y0) < lib.ClipYMax
                ) {
                    uint8_t color = sprite->data[y_pos * in_size + x_pos];
                    if (color != lib.Transparent_Color) {
                        *plot = color;
                    }
                }
            }
            plot++;
            xc_cos += cos_jump;
            xc_sin += sin_jump;
        }
        plot += GFZ_LCD_WIDTH - out_size;
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
void GraphX::gfz_RLETSprite_NoClip(const gfz_rletsprite_t *sprite, const uint32_t x, const uint8_t y) {
    if (sprite->width == 0 || sprite->height == 0) {
        return;
    }
    gfz_Wait();
    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + (GFZ_LCD_WIDTH * y));
    const uint32_t dst_jump = GFZ_LCD_WIDTH - sprite->width;

    for (uint8_t posY = 0; posY < sprite->height; posY++) {
        uint32_t posX = 0;
        while (posX < sprite->width) {
            const uint8_t jump_TP = *src_buf++;
            posX += jump_TP;
            dst_buf += jump_TP;

            if (posX >= sprite->width) {
                break;
            }

            const uint8_t len = *src_buf++;
            posX += len;
            for (uint8_t r = 0; r < len; r++) {
                *dst_buf++ = *src_buf++;
            }
        }
        dst_buf += dst_jump;
    }
}

template<>
void GraphX::gfz_RLETSprite(const gfz_rletsprite_t *__restrict sprite, const int32_t x, const int32_t y) {
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

    const uint8_t* src_buf = sprite->data;
    uint8_t* dst_buf = (uint8_t*)RAM_ADDRESS(CurrentBuffer) + (x + (GFZ_LCD_WIDTH * y));
    const uint32_t dst_jump = GFZ_LCD_WIDTH - sprite->width;

    for (uint8_t posY = 0; posY < sprite->height; posY++) {
        uint32_t posX = 0;
        while (posX < sprite->width) {
            const uint8_t jump_TP = *src_buf++;
            posX += jump_TP;
            dst_buf += jump_TP;

            if (posX >= sprite->width) {
                break;
            }

            const uint8_t len = *src_buf++;
            posX += len;
            for (uint8_t r = 0; r < len; r++) {
                if (
                    (int32_t)posX + x >= lib.ClipXMin &&
                    (int32_t)posX + x < lib.ClipXMax &&
                    (int32_t)posY + y >= lib.ClipYMin &&
                    (int32_t)posY + y < lib.ClipYMax
                ) {
                    *dst_buf = *src_buf;
                }
                ++dst_buf;
                ++src_buf;
            }
        }
        dst_buf += dst_jump;
    }
}

#if 1
template<>
gfz_sprite_t *GraphX::gfz_ConvertFromRLETSprite(const gfz_rletsprite_t *sprite_in, gfz_sprite_t *sprite_out) {
    uint8_t IYL = sprite_in->width;
    uint8_t IYH = sprite_in->height;
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;

    const uint8_t *HL = sprite_in->data;
    uint8_t* DE = sprite_out->data;
    uint8_t A, B, C;

ConvertFromRLETSprite_Row:
    A = IYL;

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
    --IYH;
    if (IYH != 0) {
        goto ConvertFromRLETSprite_Row;
    }
    return sprite_out;
}
#else
template<>
gfz_sprite_t *GraphX::gfz_ConvertFromRLETSprite(const gfz_rletsprite_t *sprite_in, gfz_sprite_t *sprite_out) {
    sprite_out->width = sprite_in->width;
    sprite_out->height = sprite_in->height;
    const uint8_t* src_buf = sprite_in->data;
    uint8_t* dst_buf = sprite_out->data;
    const uint32_t dst_jump = sprite_in->width - 1;

    for (uint8_t posY = 0; posY < sprite_in->height; posY++) {
        uint8_t posX = 0;
        while (posX < sprite_in->width) {
            const uint8_t jump_TP = *src_buf;
            posX += jump_TP;
            memset(dst_buf, lib.Transparent_Color, jump_TP);
            dst_buf += jump_TP;
            src_buf++;

            if (posX >= sprite_in->width) {
                break;
            }

            const uint8_t len = *src_buf;
            src_buf++;
            posX += len;
            memcpy(dst_buf, src_buf, len);
            src_buf += len;
            dst_buf += len;
        }
        dst_buf -= dst_jump;
    }
    return sprite_out;
}
#endif

template<>
gfz_rletsprite_t *GraphX::gfz_ConvertToRLETSprite(const gfz_sprite_t *sprite_in, gfz_rletsprite_t *sprite_out) {
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
gfz_rletsprite_t *GraphX::gfz_ConvertToNewRLETSprite(const gfz_sprite_t *sprite_in, void *(*malloc_routine)(size_t)) {
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

#include "lib_graphx.hpp"
