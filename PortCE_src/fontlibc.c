
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <PortCE.h>
#include <fontlibc.h>
#include <graphx.h>
#include <stdio.h>

static char const * fontPackHeaderString = "FONTPACK";

static unsigned int textXMin = 0;
static uint8_t textYMin = 0;

static unsigned int textXMax = GFX_LCD_WIDTH;
static uint8_t textYMax = GFX_LCD_HEIGHT;

static unsigned int textX = 0;
static uint8_t textY = 0;

#define bEnableAutoWrap 0
#define mEnableAutoWrap (1 << bEnableAutoWrap)
#define bAutoClearToEOL 1
#define mAutoClearToEOL	(1 << bAutoClearToEOL)
#define bPreclearNewline 2
#define mPreclearNewline (1 << bPreclearNewline)
#define bWasNewline 7
#define mWasNewline (1 << bWasNewline)
#define bAutoScroll 3
#define mAutoScroll (1 << bAutoScroll)

static bool textTransparentMode = false;
static uint8_t newlineControl = mEnableAutoWrap | mAutoClearToEOL;
static unsigned char const * strReadPtr = NULL;
static size_t charactersLeft = 0;
static unsigned char alternateStopCode = '\0';
static unsigned char firstPrintableCodePoint = 0x10;
static unsigned char newLineCode = 0x0A;
static unsigned char readCharacter = '\0';
static unsigned char drawIntMinus = '-';
static unsigned char drawIntZero = '0';

static uint8_t TEXT_FG_COLOR = 0;
static uint8_t TEXT_BG_COLOR = 255;

static fontlib_font_t const * currentFontRoot = NULL;
static fontlib_font_t currentFont = {0};
static uint8_t const * widthsTablePtr = NULL;
static uint8_t const * bitmapsTablePtr = NULL;

#define CurrentBuffer (*(uint8_t * volatile *)RAM_ADDRESS(0xE30014))

//------------------------------------------------------------------------------
// utilities
//------------------------------------------------------------------------------

static void test_fontlibc_state(void) {
    if (
        (textXMin > textXMax || textYMin > textYMax) ||
        (textXMax > GFX_LCD_WIDTH || textXMax > GFX_LCD_WIDTH)
    ) {
        printf(
            "fontlibc: invalid window bounds: min(%u, %u) max(%u, %u)\n",
            textXMin, textYMin, textXMax, textYMax
        );
    }
    if (textX >= GFX_LCD_WIDTH || textY > GFX_LCD_HEIGHT) {
        printf(
            "fontlibc: cursor out of bounds: (%u, %u)\n",
            textX, textY
        );
    }
}

//------------------------------------------------------------------------------
// getters and setters
//------------------------------------------------------------------------------

/* window */

ti_unsigned_int fontlib_GetWindowXMin(void) {
    return (ti_unsigned_int)textXMin;
}

uint8_t fontlib_GetWindowYMin(void) {
    return textYMin;
}

ti_unsigned_int fontlib_GetWindowWidth(void) {
    return (ti_unsigned_int)(textXMax - textXMin);
}

uint8_t fontlib_GetWindowHeight(void) {
    return textYMax - textYMin;
}

void fontlib_SetWindowFullScreen(void) {
    textXMin = 0;
    textYMin = 0;
    textXMax = GFX_LCD_WIDTH;
    textYMax = GFX_LCD_HEIGHT;
}

void fontlib_SetWindow(unsigned int x_min, uint8_t y_min, unsigned int width, uint8_t height) {
    textXMin = x_min;
    textYMin = y_min;
    textXMax = x_min + width;
    textYMax = y_min + height;
    test_fontlibc_state();
}

/* cursor */

ti_unsigned_int fontlib_GetCursorX(void) {
    return (ti_unsigned_int)textX;
}

uint8_t fontlib_GetCursorY(void) {
    return textY;
}

void fontlib_SetCursorPosition(unsigned int x, uint8_t y) {
    textX = x;
    textY = y;
    test_fontlibc_state();
}

void fontlib_ShiftCursorPosition(int x, int y) {
    // assembly truncates to 16 bit
    textX = (uint16_t)(textX + x);
    textY = (uint8_t)(textY + y);
    test_fontlibc_state();
}

void fontlib_HomeUp() {
    textY = 0;
}

void fontlib_Home() {
    textX = 0;
    textY = 0;
}

/* color */

uint8_t fontlib_GetForegroundColor(void) {
    return TEXT_FG_COLOR;
}

uint8_t fontlib_GetBackgroundColor(void) {
    return TEXT_BG_COLOR;
}

void fontlib_SetForegroundColor(uint8_t color) {
    TEXT_FG_COLOR = color;
}

void fontlib_SetBackgroundColor(uint8_t color) {
    TEXT_BG_COLOR = color;
}

void fontlib_SetColors(uint8_t forecolor, uint8_t backcolor) {
    fontlib_SetForegroundColor(forecolor);
    fontlib_SetBackgroundColor(backcolor);
}

/* font config */

bool fontlib_GetTransparency(void) {
    return textTransparentMode;
}

void fontlib_SetTransparency(bool transparency) {
    textTransparentMode = transparency;
}

void fontlib_SetNewlineOptions(uint8_t options) {
    newlineControl = options;
}

uint8_t fontlib_GetNewlineOptions(void) {
    return newlineControl;
}

char *fontlib_GetLastCharacterRead(void) {
    return (char*)strReadPtr;
}

size_t fontlib_GetCharactersRemaining(void) {
    return charactersLeft;
}

size_t fontlib_GetTotalGlyphs(void) {
    return (currentFont.total_glyphs == 0) ? 256 : currentFont.total_glyphs;
}

char fontlib_GetFirstGlyph(void) {
    return (char)currentFont.first_glyph;
}

void fontlib_SetNewlineCode(char code_point) {
    newLineCode = code_point;
}

char fontlib_GetNewlineCode(void) {
    return (char)newLineCode;
}

void fontlib_SetAlternateStopCode(char code_point) {
    alternateStopCode = code_point;
}

char fontlib_GetAlternateStopCode(void) {
    return (char)alternateStopCode;
}

void fontlib_SetFirstPrintableCodePoint(char code_point) {
    currentFont.first_glyph = code_point;
}

char fontlib_GetFirstPrintableCodePoint(void) {
    return (char)currentFont.first_glyph;
}

void fontlib_SetDrawIntCodePoints(char minus, char zero) {
    drawIntMinus = minus;
    drawIntZero = zero;
}

char fontlib_GetDrawIntMinus(void) {
    return (char)drawIntMinus;
}

char fontlib_GetDrawIntZero(void) {
    return (char)drawIntZero;
}

void fontlib_SetLineSpacing(uint8_t space_above, uint8_t space_below) {
    currentFont.space_above = space_above;
    currentFont.space_below = space_below;
}

uint8_t fontlib_GetSpaceAbove(void) {
    return currentFont.space_above;
}

uint8_t fontlib_GetSpaceBelow(void) {
    return currentFont.space_below;
}

void fontlib_SetItalicSpacingAdjustment(uint8_t italic_spacing_adjustment) {
    currentFont.italic_space_adjust = italic_spacing_adjustment;
}

uint8_t fontlib_GetItalicSpacingAdjustment(void) {
    return currentFont.italic_space_adjust;
}

uint8_t fontlib_GetCurrentFontHeight(void) {
    return currentFont.height + currentFont.space_above;
}

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

bool fontlib_SetFont(const fontlib_font_t *font_data, fontlib_load_options_t flags) {
    if (font_data->fontVersion != 0) {
        return false;
    }
    currentFontRoot = font_data;
    memcpy(&currentFont, font_data, sizeof(fontlib_font_t));
    widthsTablePtr = NULL;
    bitmapsTablePtr = NULL;
    if (currentFont.height == 0 || currentFont.height >= 0x80) {
        return false;
    }
    // magic value
    const uint8_t Space_Limit = 63;
    if (Space_Limit < currentFont.space_above) {
        return false;
    }
    if (Space_Limit < currentFont.space_below) {
        return false;
    }
    const size_t Maximum_Reasonable_Font_Size = 0xFF00;
    if ((size_t)currentFont.widths_table >= Maximum_Reasonable_Font_Size) {
        return false;
    }
    widthsTablePtr = (uint8_t * const)currentFontRoot + (size_t)currentFont.widths_table;
    if ((size_t)currentFont.bitmaps >= Maximum_Reasonable_Font_Size) {
        return false;
    }
    bitmapsTablePtr = (uint8_t * const)currentFontRoot + (size_t)currentFont.bitmaps;
    if (flags == FONTLIB_IGNORE_LINE_SPACING || (int)flags != 0) {
        currentFont.space_above = 0;
        currentFont.space_below = 0;
    }
    return true;
}

bool fontlib_ValidateCodePoint(char code_point) {
    unsigned char c = code_point;
    if (c < fontlib_GetFirstGlyph()) {
        return false;
    }
    if (c >= fontlib_GetTotalGlyphs() - fontlib_GetFirstGlyph()) {
        return false;
    }
    return true;
}

uint8_t fontlib_GetGlyphWidth(char codepoint);

ti_unsigned_int fontlib_GetStringWidthL(const char *str, size_t max_characters);

ti_unsigned_int fontlib_GetStringWidth(const char *str) {
    return fontlib_GetStringWidthL(str, UINT24_MAX);
}

ti_unsigned_int fontlib_DrawGlyph(uint8_t glyph);

ti_unsigned_int fontlib_DrawStringL(const char *str, size_t max_characters);

ti_unsigned_int fontlib_DrawString(const char *str) {
    return fontlib_DrawStringL(str, UINT24_MAX);
}

void fontlib_DrawUInt(uint24_t n, uint8_t length) {
    if (length < 1 || length > 8) {
        printf("fontlib_Draw(U)Int: invalid length %u\n", length);
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
        fontlib_DrawGlyph(drawIntZero);
    }
    uint8_t digit;
    switch (digit_count) {
        case 8: // 10'000'000 <= n <= 16'777'215
            fontlib_DrawGlyph(1 + drawIntZero);
        case 7:
            digit = ((n / 1000000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 6:
            digit = ((n / 100000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 5:
            digit = ((n / 10000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 4:
            digit = ((n / 1000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 3:
            digit = ((n / 100) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 2:
            digit = ((n / 10) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 1:
            digit = (n % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
        case 0:
        return;
    }
}

void fontlib_DrawInt(int24_t n, uint8_t length) {
    uint24_t N = (uint24_t)n;
    if (n < 0) {
        fontlib_DrawGlyph(drawIntMinus);
        N = -N;
        if (length > 1) {
            length--;
        }
    }
    fontlib_DrawUInt(N, length);
}

void fontlib_ClearEOL(void);

void fontlib_ClearWindow(void);

bool fontlib_Newline(void);

void gontlib_ScrollWindowUp(void) {
    const size_t width = fontlib_GetWindowWidth();
    const uint8_t height = fontlib_GetCurrentFontHeight();
    int lines = (textYMax - textYMin - height);
    if (lines <= 0) {
        return;
    }
    if (lines - (int)height < 0) {
        return;
    }
    const size_t line_count = (size_t)lines;
    const size_t x_pos = (size_t)textXMin;
    const uint8_t y_pos = (uint8_t)textYMin;

    uint8_t const * src = CurrentBuffer + x_pos + (y_pos * GFX_LCD_WIDTH);
    uint8_t * dst = (uint8_t*)src + GFX_LCD_WIDTH;
    gfx_Wait();
    for (size_t i = 1; i < width; i++) {
        memcpy(dst, src, line_count);
        dst += GFX_LCD_WIDTH;
        src += GFX_LCD_WIDTH;
    }
}

void gontlib_ScrollWindowDown(void) {
    const size_t width = fontlib_GetWindowWidth();
    const uint8_t height = fontlib_GetCurrentFontHeight();
    int lines = (textYMax - textYMin - height);
    if (lines <= 0) {
        return;
    }
    if (lines - (int)height < 0) {
        return;
    }
    const size_t line_count = (size_t)lines;
    const size_t x_pos = (size_t)textXMin;
    const uint8_t y_pos = (uint8_t)textYMin;

    uint8_t * dst = CurrentBuffer + x_pos + (y_pos * GFX_LCD_WIDTH);
    uint8_t const * src = (uint8_t const *)dst + GFX_LCD_WIDTH;
    gfx_Wait();
    for (size_t i = 1; i < width; i++) {
        memcpy(dst, src, line_count);
        dst += GFX_LCD_WIDTH;
        src += GFX_LCD_WIDTH;
    }
}

char *fontlib_GetFontPackName(const char *appvar_name);

fontlib_font_t *fontlib_GetFontByIndexRaw(const fontlib_font_pack_t *font_pack, uint8_t index);

fontlib_font_t *fontlib_GetFontByIndex(const char *font_pack_name, uint8_t index);

fontlib_font_t *fontlib_GetFontByStyleRaw(const fontlib_font_pack_t *font_pack, uint8_t size_min, uint8_t size_max, uint8_t weight_min, uint8_t weight_max, uint8_t style_bits_set, uint8_t style_bits_reset);

fontlib_font_t *fontlib_GetFontByStyle(const char *font_pack_name, uint8_t size_min, uint8_t size_max, uint8_t weight_min, uint8_t weight_max, uint8_t style_bits_set, uint8_t style_bits_reset);
