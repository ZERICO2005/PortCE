#ifndef LIB_GRAPHX_HPP
#define LIB_GRAPHX_HPP

void gfx_Begin(void) {
    lib.gfz_Begin();
}

void gfx_End(void) {
    lib.gfz_End();
}

gfx_sprite_t *gfx_AllocSprite(
    uint8_t width,
    uint8_t height,
    void *(*malloc_routine)(size_t)
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_AllocSprite(width, height, malloc_routine)
    );
}

void gfx_Tilemap(
    const gfx_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_Tilemap(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

void gfx_Tilemap_NoClip(
    const gfx_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_Tilemap_NoClip(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

void gfx_TransparentTilemap(
    const gfx_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_TransparentTilemap(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

void gfx_TransparentTilemap_NoClip(
    const gfx_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_TransparentTilemap_NoClip(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

uint8_t *gfx_TilePtr(
    const gfx_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    return lib.gfz_TilePtr(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

uint8_t *gfx_TilePtrMapped(
    const gfx_tilemap_t *tilemap,
    uint8_t col,
    uint8_t row
) {
    return lib.gfz_TilePtrMapped(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        col, row
    );
}

uint8_t gfx_SetColor(uint8_t index) {
    return lib.gfz_SetColor(index);
}

uint8_t gfx_SetTransparentColor(uint8_t index) {
    return lib.gfz_SetTransparentColor(index);
}

void gfx_SetDefaultPalette(gfx_mode_t mode) {
    lib.gfz_SetDefaultPalette(static_cast<gfz_mode_t>(mode));
}

void gfx_SetPalette(const void *palette, size_t size, uint8_t offset) {
    lib.gfz_SetPalette(palette, size, offset);
}

void gfx_FillScreen(uint8_t index) {
    lib.gfz_FillScreen(index);
}

void gfx_ZeroScreen(void) {
    lib.gfz_ZeroScreen();
}

void gfx_SetPixel(ti_uint x, uint8_t y) {
    lib.gfz_SetPixel(x, y);
}

uint8_t gfx_GetPixel(ti_uint x, uint8_t y) {
    return lib.gfz_GetPixel(x, y);
}

void gfx_Line(
    ti_int x0, ti_int y0,
    ti_int x1, ti_int y1
) {
    lib.gfz_Line(x0, y0, x1, y1);
}

void gfx_Line_NoClip(
    ti_uint x0, uint8_t y0,
    ti_uint x1, uint8_t y1
) {
    lib.gfz_Line_NoClip(x0, y0, x1, y1);
}

void gfx_HorizLine(ti_int x, ti_int y, ti_int length) {
    lib.gfz_HorizLine(x, y, length);
}

void gfx_HorizLine_NoClip(ti_uint x, uint8_t y, ti_uint length) {
    lib.gfz_HorizLine_NoClip(x, y, length);
}

void gfx_VertLine(ti_int x, ti_int y, ti_int length) {
    lib.gfz_VertLine(x, y, length);
}

void gfx_VertLine_NoClip(ti_uint x, uint8_t y, ti_uint length) {
    lib.gfz_VertLine_NoClip(x, y, length);
}

void gfx_Rectangle(
    ti_int x, ti_int y,
    ti_int width, ti_int height
) {
    lib.gfz_Rectangle(x, y, width, height);
}

void gfx_Rectangle_NoClip(
    ti_uint x, uint8_t y,
    ti_uint width, uint8_t height
) {
    lib.gfz_Rectangle_NoClip(x, y, width, height);
}

void gfx_FillRectangle(
    ti_int x, ti_int y,
    ti_int width, ti_int height
) {
    lib.gfz_FillRectangle(x, y, width, height);
}

void gfx_FillRectangle_NoClip(
    ti_uint x, uint8_t y,
    ti_uint width, uint8_t height
) {
    lib.gfz_FillRectangle_NoClip(x, y, width, height);
}

void gfx_Circle(
    ti_int x, ti_int y,
    ti_uint radius
) {
    lib.gfz_Circle(x, y, radius);
}

void gfx_FillCircle(
    ti_int x, ti_int y,
    ti_uint radius
) {
    lib.gfz_FillCircle(x, y, radius);
}

void gfx_FillCircle_NoClip(
    ti_uint x, uint8_t y,
    ti_uint radius
) {
    lib.gfz_FillCircle_NoClip(x, y, radius);
}

void gfx_FillEllipse_NoClip(
    ti_uint x, ti_uint y,
    uint8_t a, uint8_t b
) {
    lib.gfz_FillEllipse_NoClip(x, y, a, b);
}

void gfx_FillEllipse(
    ti_int x, ti_int y,
    ti_uint a, ti_uint b
) {
    lib.gfz_FillEllipse(x, y, a, b);
}

void gfx_Ellipse_NoClip(
    ti_uint x, ti_uint y,
    uint8_t a, uint8_t b
) {
    lib.gfz_Ellipse_NoClip(x, y, a, b);
}

void gfx_Ellipse(
    ti_int x, ti_int y,
    ti_uint a, ti_uint b
) {
    lib.gfz_Ellipse(x, y, a, b);
}

void gfx_Polygon(const ti_int *points, size_t num_points) {
    lib.gfz_Polygon(points, num_points);
}

void gfx_Polygon_NoClip(const ti_int *points, size_t num_points) {
    lib.gfz_Polygon_NoClip(points, num_points);
}

void gfx_FillTriangle(
    ti_int x0, ti_int y0,
    ti_int x1, ti_int y1,
    ti_int x2, ti_int y2
) {
    lib.gfz_FillTriangle(
        x0, y0,
        x1, y1,
        x2, y2
    );
}

void gfx_FillTriangle_NoClip(
    ti_int x0, ti_int y0,
    ti_int x1, ti_int y1,
    ti_int x2, ti_int y2
) {
    lib.gfz_FillTriangle_NoClip(
        x0, y0,
        x1, y1,
        x2, y2
    );
}

void gfx_SetDraw(uint8_t location) {
    lib.gfz_SetDraw(location);
}

uint8_t gfx_GetDraw(void) {
    return lib.gfz_GetDraw();
}

void gfx_SwapDraw(void) {
    lib.gfz_SwapDraw();
}

void gfx_Wait(void) {
    lib.gfz_Wait();
}

void gfx_Blit(gfx_location_t src) {
    lib.gfz_Blit(static_cast<gfz_location_t>(src));
}

void gfx_BlitLines(gfx_location_t src, uint8_t y_loc, uint8_t num_rows) {
    lib.gfz_BlitRows(static_cast<gfz_location_t>(src), y_loc, num_rows);
}

void gfx_BlitRectangle(
    gfx_location_t src,
    ti_uint x, uint8_t y,
    ti_uint width, ti_uint height
) {
    lib.gfz_BlitRectangle(
        static_cast<gfz_location_t>(src),
        x, y,
        width, height
    );
}

void gfx_CopyRectangle(
    gfx_location_t src, gfx_location_t dst,
    ti_uint src_x, uint8_t src_y,
    ti_uint dst_x, uint8_t dst_y,
    ti_uint width, uint8_t height
) {
    lib.gfz_CopyRectangle(
        static_cast<gfz_location_t>(src), static_cast<gfz_location_t>(dst),
        src_x, src_y,
        dst_x, dst_y,
        width, height
    );
}

void gfx_SetTextScale(uint8_t width_scale, uint8_t height_scale) {
    lib.gfz_SetTextScale(width_scale, height_scale);
}

void gfx_PrintChar(const char c) {
    lib.gfz_PrintChar(c);
}

void gfx_PrintInt(int24_t n, uint8_t length) {
    lib.gfz_PrintInt(n, length);
}

void gfx_PrintUInt(uint24_t n, uint8_t length) {
    lib.gfz_PrintUInt(n, length);
}

void gfx_PrintString(const char *string) {
    lib.gfz_PrintString(string);
}

void gfx_PrintStringXY(const char *string, ti_int x, ti_int y) {
    lib.gfz_PrintStringXY(
        string,
        x, y
    );
}

ti_int gfx_GetTextX(void) {
    return static_cast<ti_int>(lib.gfz_GetTextX());
}

ti_int gfx_GetTextY(void) {
    return static_cast<ti_int>(lib.gfz_GetTextY());
}

void gfx_SetTextXY(ti_int x, ti_int y) {
    lib.gfz_SetTextXY(x, y);
}

void gfx_SetTextConfig(uint8_t config) {
    lib.gfz_SetTextConfig(config);
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

void gfx_Sprite(
    const gfx_sprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_Sprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

void gfx_Sprite_NoClip(
    const gfx_sprite_t *sprite,
    ti_uint x, uint8_t y
) {
    lib.gfz_Sprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

void gfx_TransparentSprite(
    const gfx_sprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_TransparentSprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

void gfx_TransparentSprite_NoClip(
    const gfx_sprite_t *sprite,
    ti_uint x,uint8_t y
) {
    lib.gfz_TransparentSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

gfx_sprite_t *gfx_GetSprite(
    gfx_sprite_t *sprite_buffer,
    ti_int x, ti_int y
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_GetSprite(
            reinterpret_cast<gfz_sprite_t*>(sprite_buffer),
            x, y
        )
    );
}

void gfx_ScaledSprite_NoClip(
    const gfx_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t width_scale, uint8_t height_scale
) {
    lib.gfz_ScaledSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        width_scale, height_scale
    );
}

void gfx_ScaledTransparentSprite_NoClip(
    const gfx_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t width_scale, uint8_t height_scale
) {
    lib.gfz_ScaledTransparentSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        width_scale, height_scale
    );
}

uint8_t gfx_RotatedScaledTransparentSprite_NoClip(
    const gfx_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledTransparentSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

uint8_t gfx_RotatedScaledSprite_NoClip(
    const gfx_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

uint8_t gfx_RotatedScaledTransparentSprite(
    const gfx_sprite_t *sprite,
    ti_int x, ti_int y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledTransparentSprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

uint8_t gfx_RotatedScaledSprite(
    const gfx_sprite_t *sprite,
    ti_int x, ti_int y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledSprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

gfx_sprite_t *gfx_FlipSpriteX(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_FlipSpriteX(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_sprite_t *gfx_FlipSpriteY(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_FlipSpriteY(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_sprite_t *gfx_RotateSpriteC(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_RotateSpriteC(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_sprite_t *gfx_RotateSpriteCC(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_RotateSpriteCC(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_sprite_t *gfx_RotateSpriteHalf(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_RotateSpriteHalf(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_sprite_t *gfx_ScaleSprite(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_ScaleSprite(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_sprite_t *gfx_RotateScaleSprite(
    const gfx_sprite_t *__restrict sprite_in,
    gfx_sprite_t *__restrict sprite_out,
    uint8_t angle,
    uint8_t scale
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_RotateScaleSprite(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out),
            angle, scale
        )
    );
}

gfx_sprite_t *gfx_GetSpriteChar(char c) {
    return reinterpret_cast<gfx_sprite_t*>(lib.gfz_GetSpriteChar(c));
}

uint8_t *gfx_SetFontData(const uint8_t *data) {
    return lib.gfz_SetFontData(data);
}

uint8_t *gfx_SetCharData(uint8_t index, const uint8_t *data) {
    return lib.gfz_SetCharData(index, data);
}

void gfx_SetFontSpacing(const uint8_t *spacing) {
    lib.gfz_SetFontSpacing(spacing);
}

uint8_t gfx_SetFontHeight(uint8_t height) {
    return lib.gfz_SetFontHeight(height);
}

void gfx_SetMonospaceFont(uint8_t spacing) {
    lib.gfz_SetMonospaceFont(spacing);
}

ti_uint gfx_GetStringWidth(const char *string) {
    return static_cast<ti_uint>(lib.gfz_GetStringWidth(string));
}

ti_uint gfx_GetCharWidth(const char c) {
    return static_cast<ti_uint>(lib.gfz_GetCharWidth(c));
}

void gfx_SetClipRegion(ti_int xmin, ti_int ymin, ti_int xmax, ti_int ymax) {
    lib.gfz_SetClipRegion(xmin, ymin, xmax, ymax);
}

bool gfx_GetClipRegion(gfx_region_t *region) {
    return lib.gfz_GetClipRegion(reinterpret_cast<gfz_region_t*>(region));
}

void gfx_ShiftDown(uint8_t pixels) {
    lib.gfz_ShiftDown(pixels);
}

void gfx_ShiftUp(uint8_t pixels) {
    lib.gfz_ShiftUp(pixels);
}

void gfx_ShiftLeft(ti_uint pixels) {
    lib.gfz_ShiftLeft(pixels);
}

void gfx_ShiftRight(ti_uint pixels) {
    lib.gfz_ShiftRight(pixels);
}

uint16_t gfx_Lighten(uint16_t color, uint8_t amount) {
    return lib.gfz_Lighten(color, amount);
}

uint16_t gfx_Darken(uint16_t color, uint8_t amount) {
    return lib.gfz_Darken(color, amount);
}

void gfx_FloodFill(ti_uint x, uint8_t y, uint8_t color) {
    lib.gfz_FloodFill(x, y, color);
}

void gfx_RLETSprite(
    const gfx_rletsprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_RLETSprite(
        reinterpret_cast<const gfz_rletsprite_t*>(sprite),
        x, y
    );
}

void gfx_RLETSprite_NoClip(
    const gfx_rletsprite_t *sprite,
    ti_uint x, uint8_t y
) {
    lib.gfz_RLETSprite_NoClip(
        reinterpret_cast<const gfz_rletsprite_t*>(sprite),
        x, y
    );
}

gfx_sprite_t *gfx_ConvertFromRLETSprite(
    const gfx_rletsprite_t *sprite_in,
    gfx_sprite_t *sprite_out
) {
    return reinterpret_cast<gfx_sprite_t*>(
        lib.gfz_ConvertFromRLETSprite(
            reinterpret_cast<const gfz_rletsprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_rletsprite_t *gfx_ConvertToRLETSprite(
    const gfx_sprite_t *sprite_in,
    gfx_rletsprite_t *sprite_out
) {
    return reinterpret_cast<gfx_rletsprite_t*>(
        lib.gfz_ConvertToRLETSprite(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_rletsprite_t *__restrict>(sprite_out)
        )
    );
}

gfx_rletsprite_t *gfx_ConvertToNewRLETSprite(
    const gfx_sprite_t *sprite_in,
    void *(*malloc_routine)(size_t)
) {
    return reinterpret_cast<gfx_rletsprite_t*>(
        lib.gfz_ConvertToNewRLETSprite(
            reinterpret_cast<const gfz_sprite_t*>(sprite_in),
            malloc_routine
        )
    );
}

#endif /* LIB_GRAPHX_HPP */
