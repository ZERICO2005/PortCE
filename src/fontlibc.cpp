
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <PortCE.h>
#include <fontlibc.h>
#include <fileioc.h>
#include <graphx.h>
#include <stdio.h>
#include <sys/lcd.h>

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
#define mAutoClearToEOL (1 << bAutoClearToEOL)
#define bPreclearNewline 2
#define mPreclearNewline (1 << bPreclearNewline)
#define bWasNewline 7
#define mWasNewline (1 << bWasNewline)
#define bAutoScroll 3
#define mAutoScroll (1 << bAutoScroll)

static bool textTransparentMode = false;
static uint8_t newlineControl = mEnableAutoWrap | mAutoClearToEOL;
static char const * strReadPtr = nullptr;
static size_t charactersLeft = 0;
static unsigned char alternateStopCode = '\0';
static unsigned char firstPrintableCodePoint = 0x10;
static unsigned char drawNewLine = '\n';
static unsigned char drawIntMinus = '-';
static unsigned char drawIntZero = '0';

static uint8_t TEXT_FG_COLOR = 0;
static uint8_t TEXT_BG_COLOR = 255;

static fontlib_font_t const * currentFontRoot = nullptr;
static fontlib_font_t currentFont = {};
static uint8_t const * widthsTablePtr = nullptr;
static uint16_t const * bitmapsTablePtr = nullptr;

inline uint8_t *get_vBuffer() {
    return static_cast<uint8_t*>(RAM_ADDRESS(lcd_LpBase));
}

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
    if (textX >= GFX_LCD_WIDTH || textY >= GFX_LCD_HEIGHT) {
        printf(
            "fontlibc: cursor out of bounds: (%u, %u)\n",
            textX, textY
        );
    }
}

static void util_ClearRect(int x_pos, int y_pos, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    uint8_t * dst = get_vBuffer() + x_pos + (y_pos * GFX_LCD_WIDTH);
    gfx_Wait();
    for (int y = 0; y < height; y++) {
        memset(dst, TEXT_BG_COLOR, static_cast<size_t>(width));
        dst += GFX_LCD_WIDTH;
    }
}

//------------------------------------------------------------------------------
// getters and setters
//------------------------------------------------------------------------------

/* window */

ti_uint fontlib_GetWindowXMin(void) {
    return static_cast<ti_uint>(textXMin);
}

uint8_t fontlib_GetWindowYMin(void) {
    return textYMin;
}

ti_uint fontlib_GetWindowWidth(void) {
    return static_cast<ti_uint>(textXMax - textXMin);
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

ti_uint fontlib_GetCursorX(void) {
    return static_cast<ti_uint>(textX);
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
    textX = static_cast<uint16_t>(textX + static_cast<unsigned>(x));
    textY = static_cast<uint8_t>(textY + static_cast<unsigned>(y));
    test_fontlibc_state();
}

void fontlib_HomeUp() {
    textY = textYMin;
}

void fontlib_Home() {
    textX = textXMin;
    textY = textYMin;
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
    return const_cast<char*>(strReadPtr);
}

size_t fontlib_GetCharactersRemaining(void) {
    return charactersLeft;
}

size_t fontlib_GetTotalGlyphs(void) {
    return (currentFont.total_glyphs == 0) ? 256 : currentFont.total_glyphs;
}

char fontlib_GetFirstGlyph(void) {
    return static_cast<char>(currentFont.first_glyph);
}

void fontlib_SetNewlineCode(char code_point) {
    drawNewLine = static_cast<unsigned char>(code_point);
}

char fontlib_GetNewlineCode(void) {
    return static_cast<char>(drawNewLine);
}

void fontlib_SetAlternateStopCode(char code_point) {
    alternateStopCode = static_cast<unsigned char>(code_point);
}

char fontlib_GetAlternateStopCode(void) {
    return static_cast<char>(alternateStopCode);
}

void fontlib_SetFirstPrintableCodePoint(char code_point) {
    currentFont.first_glyph = static_cast<unsigned char>(code_point);
}

char fontlib_GetFirstPrintableCodePoint(void) {
    return static_cast<char>(currentFont.first_glyph);
}

void fontlib_SetDrawIntCodePoints(char minus, char zero) {
    drawIntMinus = static_cast<unsigned char>(minus);
    drawIntZero = static_cast<unsigned char>(zero);
}

char fontlib_GetDrawIntMinus(void) {
    return static_cast<char>(drawIntMinus);
}

char fontlib_GetDrawIntZero(void) {
    return static_cast<char>(drawIntZero);
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
    widthsTablePtr = nullptr;
    bitmapsTablePtr = nullptr;
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
    if (static_cast<size_t>(currentFont.widths_table) >= Maximum_Reasonable_Font_Size) {
        return false;
    }
    widthsTablePtr = reinterpret_cast<const uint8_t*>(currentFontRoot) + static_cast<size_t>(currentFont.widths_table);
    if (static_cast<size_t>(currentFont.bitmaps) >= Maximum_Reasonable_Font_Size) {
        return false;
    }
    bitmapsTablePtr = reinterpret_cast<const uint16_t*>(
        reinterpret_cast<const uint8_t*>(currentFontRoot) + static_cast<size_t>(currentFont.bitmaps)
    );
#if 0
    if (flags == FONTLIB_IGNORE_LINE_SPACING || static_cast<int>(flags) != 0) {
        currentFont.space_above = 0;
        currentFont.space_below = 0;
    }
#else
    if (flags == FONTLIB_IGNORE_LINE_SPACING || static_cast<int>(flags) != 0 || true) {
        currentFont.space_above = 0;
        currentFont.space_below = 0;
    }
#endif
    return true;
}

bool fontlib_ValidateCodePoint(char code_point) {
    unsigned char c = static_cast<unsigned char>(code_point);
    if (c < fontlib_GetFirstGlyph()) {
        return false;
    }
    if (c >= fontlib_GetTotalGlyphs() - static_cast<unsigned char>(fontlib_GetFirstGlyph())) {
        return false;
    }
    return true;
}

uint8_t fontlib_GetGlyphWidth(char codepoint) {
    unsigned char ch = static_cast<unsigned char>(codepoint);
    if (ch < currentFont.first_glyph) {
        // invalid index
        return 0;
    }
    ch -= currentFont.first_glyph;
    if (ch >= fontlib_GetTotalGlyphs()) {
        return 0;
    }
    return widthsTablePtr[ch];
}

ti_uint fontlib_GetStringWidthL(const char *str, size_t max_characters) {
    if (max_characters > UINT24_MAX) {
        max_characters = UINT24_MAX;
    }
    charactersLeft = max_characters;
    if (*str == '\0') {
        strReadPtr = str;
        return 0;
    }
    ti_uint total_width = 0;
    for (;;) {
        if (charactersLeft == 0) {
            break;
        }
        --charactersLeft;
        unsigned char ch = static_cast<unsigned char>(*str);
        if (ch < firstPrintableCodePoint) {
            break;
        }
        if (ch == '\0') {
            break;
        }
        if (ch == alternateStopCode) {
            break;
        }
        if (ch < currentFont.first_glyph) {
            break;
        }
        ch -= currentFont.first_glyph;
        if (ch >= fontlib_GetTotalGlyphs()) {
            break;
        }
        ++str;
        int width = widthsTablePtr[ch];
        int adjust = currentFont.italic_space_adjust;
        width -= adjust;
        total_width += width;
    }
    strReadPtr = str;
    int adjust = currentFont.italic_space_adjust;
    total_width -= adjust;
    return static_cast<ti_uint>(total_width);
}

ti_uint fontlib_GetStringWidth(const char *str) {
    return fontlib_GetStringWidthL(str, UINT24_MAX);
}

// assumes ch is already decremented by currentFont.first_glyph
static uint8_t util_DrawGlyphRawKnownWidth(uint8_t ch, uint8_t *__restrict const buf, uint8_t width) {
    const uint8_t space_above = currentFont.space_above;
    const uint8_t space_below = currentFont.space_below;
    const uint8_t height = currentFont.height;

    const uint8_t bg_color = TEXT_BG_COLOR;
    const uint8_t fg_color = TEXT_FG_COLOR;
    const bool is_transparent = textTransparentMode;

    uint16_t const *__restrict const bitmap_table_ptr = bitmapsTablePtr;
    size_t bitmap_offset = bitmap_table_ptr[static_cast<unsigned char>(ch)];
    uint8_t const *__restrict src = reinterpret_cast<uint8_t const *__restrict>(currentFontRoot) + bitmap_offset;

    const uint8_t font_jump = (static_cast<uint8_t>(width - 1) >> 3) + 1;

    if (is_transparent) {
        uint8_t *__restrict dst = buf;
        const size_t line_jump = GFX_LCD_WIDTH - width;
        dst += space_above * GFX_LCD_WIDTH;
        for (uint8_t y = 0; y < height; y++) {
            uint24_t HL = *reinterpret_cast<uint24_t const *__restrict>(src);
            for (uint8_t x = 0; x < width; x++) {
                if (HL & 0x800000) {
                    *dst = fg_color;
                }
                HL <<= 1;
                dst++;
            }
            src += font_jump;
            dst += line_jump;
        }
        return width;
    }
    /* opaque */ {
        uint8_t *__restrict dst = buf;
        const size_t line_jump = GFX_LCD_WIDTH - width;
        dst += space_above * GFX_LCD_WIDTH;
        for (uint8_t y = 0; y < height; y++) {
            uint24_t HL = *reinterpret_cast<uint24_t const *__restrict>(src);
            for (uint8_t x = 0; x < width; x++) {
                if (HL & 0x800000) {
                    *dst = fg_color;
                } else {
                    *dst = bg_color;
                }
                HL <<= 1;
                dst++;
            }
            src += font_jump;
            dst += line_jump;
        }
    }

    /* draw empty lines above */ {
        uint8_t *__restrict above = buf;
        for (uint8_t y = 0; y < space_above; y++) {
            memset(above, bg_color, width);
            above += GFX_LCD_WIDTH;
        }
    }
    /* draw empty lines below */ {
        uint8_t *__restrict below = buf + (space_above + height) * GFX_LCD_WIDTH;
        for (uint8_t y = 0; y < space_below; y++) {
            memset(below, bg_color, width);
            below += GFX_LCD_WIDTH;
        }
    }
    return width;
}

static uint8_t util_DrawGlyphRaw(unsigned char ch, uint8_t *__restrict const buf) {
    ch -= currentFont.first_glyph;
    uint8_t const *__restrict const width_table_ptr = widthsTablePtr;
    const uint8_t width = width_table_ptr[ch];
    gfx_Wait();
    return util_DrawGlyphRawKnownWidth(ch, buf, width);
}

ti_uint fontlib_DrawGlyph(unsigned char ch) {
    uint8_t *__restrict buf = get_vBuffer() + textX + (textY * GFX_LCD_WIDTH);
    uint8_t width = util_DrawGlyphRaw(ch, buf);
    textX += width;
    textX -= currentFont.italic_space_adjust;
    return static_cast<ti_uint>(textX);
}

static void util_DrawStringL(const char *str, size_t max_characters) {
    newlineControl &= ~mWasNewline;
    strReadPtr = (str - 1);
    charactersLeft = max_characters;
restartX:
    gfx_Wait();
    uint8_t *__restrict buf = get_vBuffer() + textX + (textY * GFX_LCD_WIDTH);
mainLoop:
    if (charactersLeft == 0) {
        return;
    }
    --charactersLeft;
    ++strReadPtr;
    unsigned char ch = static_cast<unsigned char>(*strReadPtr);

    if (ch < firstPrintableCodePoint) {
        if (ch == '\0') {
            return;
        }
        if (ch == drawNewLine) {
            goto printNewLine;
        }
        return;
    }
    if (ch == alternateStopCode) {
        return;
    }
    if (ch < currentFont.first_glyph) {
        return;
    }
    ch -= currentFont.first_glyph;
    if (ch >= fontlib_GetTotalGlyphs()) {
        return;
    }
    {
        uint8_t width = widthsTablePtr[ch];
        int x_pos = static_cast<int>(textX + width);
        int adjust = currentFont.italic_space_adjust;
        if (static_cast<unsigned int>(x_pos) > textXMax) {
            goto newLine;
        }
        x_pos -= adjust;
        textX = static_cast<unsigned int>(x_pos);
        buf += util_DrawGlyphRawKnownWidth(ch, buf, width);
        buf -= adjust;
    }
    goto mainLoop;

//------------------------------------------------------------------------------
printNewLine:
    newlineControl |= mWasNewline;
newLine:
    if (newlineControl & mWasNewline) {
        goto doNewLine;
    }
    if (!(newlineControl & mEnableAutoWrap)) {
        return;
    }
doNewLine:
    if (fontlib_Newline()) {
        return;
    }
    bool was_new_line = (newlineControl & mWasNewline);
    newlineControl &= ~mWasNewline;
    if (was_new_line) {
        goto restartX;
    }
    --strReadPtr;
    goto restartX;
}

ti_uint fontlib_DrawStringL(const char *str, size_t max_characters) {
    if (max_characters > UINT24_MAX) {
        max_characters = UINT24_MAX;
    }
    util_DrawStringL(str, max_characters);
    return static_cast<ti_uint>(textX);
}

ti_uint fontlib_DrawString(const char *str) {
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
            [[fallthrough]];
        case 7:
            digit = ((n / 1000000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 6:
            digit = ((n / 100000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 5:
            digit = ((n / 10000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 4:
            digit = ((n / 1000) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 3:
            digit = ((n / 100) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 2:
            digit = ((n / 10) % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 1:
            digit = (n % 10) + drawIntZero;
            fontlib_DrawGlyph(digit);
            [[fallthrough]];
        case 0:
        return;
    }
}

void fontlib_DrawInt(int24_t n, uint8_t length) {
    uint24_t N = static_cast<uint24_t>(n);
    if (n < 0) {
        fontlib_DrawGlyph(drawIntMinus);
        N = -N;
        if (length > 1) {
            length--;
        }
    }
    fontlib_DrawUInt(N, length);
}

void fontlib_ClearEOL(void) {
    int width = static_cast<int>(textXMax - textX);
    if (width <= 0) {
        return;
    }
    int height = currentFont.height + currentFont.space_above + currentFont.space_below;
    if (height <= 0) {
        return;
    }
    util_ClearRect(
        static_cast<int>(textXMin),
        static_cast<int>(textYMin),
        static_cast<int>(width),
        static_cast<int>(height)
    );
}

void fontlib_ClearWindow(void) {
    int width = static_cast<int>(textXMax - textX);
    int height = static_cast<int>(textYMax - textYMin);
    util_ClearRect(
        static_cast<int>(textXMin),
        static_cast<int>(textYMin),
        static_cast<int>(width),
        static_cast<int>(height)
    );
}

bool fontlib_Newline(void) {
    if (newlineControl & mAutoClearToEOL) {
        fontlib_ClearEOL();
    }
    textX = textXMin;
    // emulate the behaviour of the assembly code
    const int font_height = currentFont.height + currentFont.space_above + currentFont.space_below;
    int height_cmp = font_height * 2;
    if (height_cmp >= 256) {
        goto noScroll;
    }
    height_cmp += textY;
    if (height_cmp >= 256) {
        goto outOfSpace;
    }
    if (height_cmp <= textYMax) {
        goto writeCursorY;
    }

outOfSpace:
    if (!(newlineControl & mAutoScroll)) {
        goto noScroll;
    }
    fontlib_ScrollWindowDown();
    goto checkPreClear;
//------------------------------------------------------------------------------
noScroll:
    if (!(newlineControl & mEnableAutoWrap)) {
        return true;
    }
    textY = textYMin;
    return true;
//------------------------------------------------------------------------------
writeCursorY:
    textY = static_cast<uint8_t>(height_cmp - font_height);
checkPreClear:
    if (!(newlineControl & mPreclearNewline)) {
        return false;
    }
    fontlib_ClearEOL();
    return false;
}

void fontlib_ScrollWindowDown(void) {
    if (textXMax <= textXMin || textYMax <= textYMin) {
        return;
    }
    const size_t width = fontlib_GetWindowWidth();
    const uint8_t height = fontlib_GetCurrentFontHeight();
    int lines = (textYMax - textYMin - height);
    if (lines <= 0) {
        return;
    }
    if (lines < static_cast<int>(height)) {
        return;
    }
    const size_t line_count = static_cast<size_t>(lines);
    const size_t x_pos = static_cast<size_t>(textXMin);
    const uint8_t y_pos = static_cast<uint8_t>(textYMin);

    uint8_t * const buf = get_vBuffer() + x_pos + (y_pos * GFX_LCD_WIDTH);
    uint8_t * dst = buf;
    uint8_t const * src = buf + (GFX_LCD_WIDTH * height);
    gfx_Wait();
    for (size_t i = 0; i < line_count; i++) {
        memcpy(dst, src, width);
        dst += GFX_LCD_WIDTH;
        src += GFX_LCD_WIDTH;
    }
}

void fontlib_ScrollWindowUp(void) {
    if (textXMax <= textXMin || textYMax <= textYMin) {
        return;
    }
    const size_t width = fontlib_GetWindowWidth();
    const uint8_t height = fontlib_GetCurrentFontHeight();
    int lines = (textYMax - textYMin - height);
    if (lines <= 0) {
        return;
    }
    if (lines < static_cast<int>(height)) {
        return;
    }
    const size_t line_count = static_cast<size_t>(lines);
    const size_t x_pos = static_cast<size_t>(textXMin);
    const uint8_t y_pos = static_cast<uint8_t>(textYMax - 1);

    uint8_t * const buf = get_vBuffer() + x_pos + (y_pos * GFX_LCD_WIDTH);
    uint8_t * dst = buf;
    uint8_t const * src = buf - (GFX_LCD_WIDTH * height);
    gfx_Wait();
    for (size_t i = 0; i < line_count; i++) {
        memcpy(dst, src, width);
        dst -= GFX_LCD_WIDTH;
        src -= GFX_LCD_WIDTH;
    }
}

// unimplemented
char *fontlib_GetFontPackName(__attribute__((__unused__)) const char *appvar_name) {
    return nullptr;
}

fontlib_font_t *fontlib_GetFontByIndexRaw(
    const fontlib_font_pack_t *font_pack,
    uint8_t index
) {
    uint8_t font_count = font_pack->fontCount;
    if (font_count <= index) {
        return nullptr;
    }
    packed_int24_t const * font_list = reinterpret_cast<packed_int24_t const *>(font_pack->font_list);
    int24_t offset;
    memcpy(&offset, &font_list[index], 3);
    return const_cast<fontlib_font_t*>(
        reinterpret_cast<fontlib_font_t const *>(
            reinterpret_cast<unsigned char const *>(font_pack) + static_cast<uintptr_t>(offset)
        )
    );
}

fontlib_font_t *fontlib_GetFontByIndex(
    const char *font_pack_name,
    uint8_t index
) {
    uint8_t handle = ti_Open(font_pack_name, "r");
    if (!handle) {
        return nullptr;
    }

    fontlib_font_pack_t *font_pack = reinterpret_cast<fontlib_font_pack_t *>(ti_GetDataPtr(handle));
    if (font_pack == nullptr) {
        ti_Close(handle);
        return nullptr;
    }

    if (memcmp(font_pack->header, fontPackHeaderString, 8) != 0) {
        ti_Close(handle);
        return nullptr;
    }

    fontlib_font_t *result = fontlib_GetFontByIndexRaw(font_pack, index);
    ti_Close(handle);
    return result;
}

fontlib_font_t *fontlib_GetFontByStyleRaw(
    const fontlib_font_pack_t *font_pack,
    uint8_t size_min,
    uint8_t size_max,
    uint8_t weight_min,
    uint8_t weight_max,
    uint8_t style_bits_set,
    uint8_t style_bits_reset
) {
    uint8_t font_count = font_pack->fontCount;
    packed_int24_t const * font_list = reinterpret_cast<packed_int24_t const *>(font_pack->font_list);
    for (uint8_t B = font_count; B --> 0;) {
        int24_t offset;
        memcpy(&offset, font_list, 3);
        fontlib_font_t const * font = reinterpret_cast<fontlib_font_t const *>(
            font_pack + static_cast<uintptr_t>(offset)
        );
        font_list++;
        if (font->height < size_min) {
            continue;
        }
        if (font->height > size_max) {
            continue;
        }
        if (font->weight < weight_min) {
            continue;
        }
        if (font->weight > weight_max) {
            continue;
        }
        if (((style_bits_set & font->style) ^ font->style) != 0) {
            continue;
        }
        if ((style_bits_reset & font->style) == 0) {
            continue;
        }
        return const_cast<fontlib_font_t*>(font);
    }
    return nullptr;
}

fontlib_font_t *fontlib_GetFontByStyle(
    const char *font_pack_name,
    uint8_t size_min,
    uint8_t size_max,
    uint8_t weight_min,
    uint8_t weight_max,
    uint8_t style_bits_set,
    uint8_t style_bits_reset
) {
    uint8_t handle = ti_Open(font_pack_name, "r");
    if (!handle) {
        return nullptr;
    }

    fontlib_font_pack_t *font_pack = reinterpret_cast<fontlib_font_pack_t *>(ti_GetDataPtr(handle));
    if (font_pack == nullptr) {
        ti_Close(handle);
        return nullptr;
    }

    if (memcmp(font_pack->header, fontPackHeaderString, 8) != 0) {
        ti_Close(handle);
        return nullptr;
    }

    fontlib_font_t *result = fontlib_GetFontByStyleRaw(
        font_pack,
        size_min, size_max,
        weight_min, weight_max,
        style_bits_set, style_bits_reset
    );
    ti_Close(handle);
    return result;
}
