#ifndef LIB_GRAPHY_HPP
#define LIB_GRAPHY_HPP

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
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_AllocSprite(width, height, malloc_routine)
    );
}

void gfy_Tilemap(
    const gfy_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_Tilemap(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

void gfy_Tilemap_NoClip(
    const gfy_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_Tilemap_NoClip(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

void gfy_TransparentTilemap(
    const gfy_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_TransparentTilemap(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

void gfy_TransparentTilemap_NoClip(
    const gfy_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    lib.gfz_TransparentTilemap_NoClip(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

uint8_t *gfy_TilePtr(
    const gfy_tilemap_t *tilemap,
    ti_uint x_offset,
    ti_uint y_offset
) {
    return lib.gfz_TilePtr(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        x_offset, y_offset
    );
}

uint8_t *gfy_TilePtrMapped(
    const gfy_tilemap_t *tilemap,
    uint8_t col,
    uint8_t row
) {
    return lib.gfz_TilePtrMapped(
        reinterpret_cast<const gfz_tilemap_t*>(tilemap),
        col, row
    );
}

uint8_t gfy_SetColor(uint8_t index) {
    return lib.gfz_SetColor(index);
}

uint8_t gfy_SetTransparentColor(uint8_t index) {
    return lib.gfz_SetTransparentColor(index);
}

void gfy_SetDefaultPalette(gfy_mode_t mode) {
    lib.gfz_SetDefaultPalette(static_cast<gfz_mode_t>(mode));
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

void gfy_SetPixel(ti_uint x, uint8_t y) {
    lib.gfz_SetPixel(x, y);
}

uint8_t gfy_GetPixel(ti_uint x, uint8_t y) {
    return lib.gfz_GetPixel(x, y);
}

void gfy_Line(
    ti_int x0, ti_int y0,
    ti_int x1, ti_int y1
) {
    lib.gfz_Line(x0, y0, x1, y1);
}

void gfy_Line_NoClip(
    ti_uint x0, uint8_t y0,
    ti_uint x1, uint8_t y1
) {
    lib.gfz_Line_NoClip(x0, y0, x1, y1);
}

void gfy_HorizLine(ti_int x, ti_int y, ti_int length) {
    lib.gfz_HorizLine(x, y, length);
}

void gfy_HorizLine_NoClip(ti_uint x, uint8_t y, ti_uint length) {
    lib.gfz_HorizLine_NoClip(x, y, length);
}

void gfy_VertLine(ti_int x, ti_int y, ti_int length) {
    lib.gfz_VertLine(x, y, length);
}

void gfy_VertLine_NoClip(ti_uint x, uint8_t y, ti_uint length) {
    lib.gfz_VertLine_NoClip(x, y, length);
}

void gfy_Rectangle(
    ti_int x, ti_int y,
    ti_int width, ti_int height
) {
    lib.gfz_Rectangle(x, y, width, height);
}

void gfy_Rectangle_NoClip(
    ti_uint x, uint8_t y,
    ti_uint width, uint8_t height
) {
    lib.gfz_Rectangle_NoClip(x, y, width, height);
}

void gfy_FillRectangle(
    ti_int x, ti_int y,
    ti_int width, ti_int height
) {
    lib.gfz_FillRectangle(x, y, width, height);
}

void gfy_FillRectangle_NoClip(
    ti_uint x, uint8_t y,
    ti_uint width, uint8_t height
) {
    lib.gfz_FillRectangle_NoClip(x, y, width, height);
}

void gfy_Circle(
    ti_int x, ti_int y,
    ti_uint radius
) {
    lib.gfz_Circle(x, y, radius);
}

void gfy_FillCircle(
    ti_int x, ti_int y,
    ti_uint radius
) {
    lib.gfz_FillCircle(x, y, radius);
}

void gfy_FillCircle_NoClip(
    ti_uint x, uint8_t y,
    ti_uint radius
) {
    lib.gfz_FillCircle_NoClip(x, y, radius);
}

void gfy_FillEllipse_NoClip(
    ti_uint x, ti_uint y,
    uint8_t a, uint8_t b
) {
    lib.gfz_FillEllipse_NoClip(x, y, a, b);
}

void gfy_FillEllipse(
    ti_int x, ti_int y,
    ti_uint a, ti_uint b
) {
    lib.gfz_FillEllipse(x, y, a, b);
}

void gfy_Ellipse_NoClip(
    ti_uint x, ti_uint y,
    uint8_t a, uint8_t b
) {
    lib.gfz_Ellipse_NoClip(x, y, a, b);
}

void gfy_Ellipse(
    ti_int x, ti_int y,
    ti_uint a, ti_uint b
) {
    lib.gfz_Ellipse(x, y, a, b);
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
        x0, y0,
        x1, y1,
        x2, y2
    );
}

void gfy_FillTriangle_NoClip(
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
    lib.gfz_Blit(static_cast<gfz_location_t>(src));
}

void gfy_BlitColumns(gfy_location_t src, ti_uint x_loc, ti_uint num_columns) {
    lib.gfz_BlitColumns(static_cast<gfz_location_t>(src), x_loc, num_columns);
}

void gfy_BlitRectangle(
    gfy_location_t src,
    ti_uint x, uint8_t y,
    ti_uint width, ti_uint height
) {
    lib.gfz_BlitRectangle(
        static_cast<gfz_location_t>(src),
        x, y,
        width, height
    );
}

void gfy_CopyRectangle(
    gfy_location_t src, gfy_location_t dst,
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
        x, y
    );
}

ti_int gfy_GetTextX(void) {
    return static_cast<ti_int>(lib.gfz_GetTextX());
}

ti_int gfy_GetTextY(void) {
    return static_cast<ti_int>(lib.gfz_GetTextY());
}

void gfy_SetTextXY(ti_int x, ti_int y) {
    lib.gfz_SetTextXY(x, y);
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
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

void gfy_Sprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_uint x, uint8_t y
) {
    lib.gfz_Sprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

void gfy_TransparentSprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_TransparentSprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

void gfy_TransparentSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_uint x,uint8_t y
) {
    lib.gfz_TransparentSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y
    );
}

gfy_sprite_t *gfy_GetSprite(
    gfy_sprite_t *sprite_buffer,
    ti_int x, ti_int y
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_GetSprite(
            reinterpret_cast<gfz_sprite_t*>(sprite_buffer),
            x, y
        )
    );
}

void gfy_ScaledSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t width_scale, uint8_t height_scale
) {
    lib.gfz_ScaledSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        width_scale, height_scale
    );
}

void gfy_ScaledTransparentSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t width_scale, uint8_t height_scale
) {
    lib.gfz_ScaledTransparentSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        width_scale, height_scale
    );
}

uint8_t gfy_RotatedScaledTransparentSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledTransparentSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

uint8_t gfy_RotatedScaledSprite_NoClip(
    const gfy_sprite_t *sprite,
    ti_uint x, uint8_t y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledSprite_NoClip(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

uint8_t gfy_RotatedScaledTransparentSprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledTransparentSprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

uint8_t gfy_RotatedScaledSprite(
    const gfy_sprite_t *sprite,
    ti_int x, ti_int y,
    uint8_t angle, uint8_t scale
) {
    return lib.gfz_RotatedScaledSprite(
        reinterpret_cast<const gfz_sprite_t*>(sprite),
        x, y,
        angle, scale
    );
}

gfy_sprite_t *gfy_FlipSpriteX(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_FlipSpriteX(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_sprite_t *gfy_FlipSpriteY(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_FlipSpriteY(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_sprite_t *gfy_RotateSpriteC(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_RotateSpriteC(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_sprite_t *gfy_RotateSpriteCC(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_RotateSpriteCC(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_sprite_t *gfy_RotateSpriteHalf(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_RotateSpriteHalf(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_sprite_t *gfy_ScaleSprite(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_ScaleSprite(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_sprite_t *gfy_RotateScaleSprite(
    const gfy_sprite_t *__restrict sprite_in,
    gfy_sprite_t *__restrict sprite_out,
    uint8_t angle,
    uint8_t scale
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_RotateScaleSprite(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out),
            angle, scale
        )
    );
}

gfy_sprite_t *gfy_GetSpriteChar(char c) {
    return reinterpret_cast<gfy_sprite_t*>(lib.gfz_GetSpriteChar(c));
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

ti_uint gfy_GetStringWidth(const char *string) {
    return static_cast<ti_uint>(lib.gfz_GetStringWidth(string));
}

ti_uint gfy_GetCharWidth(const char c) {
    return static_cast<ti_uint>(lib.gfz_GetCharWidth(c));
}

void gfy_SetClipRegion(ti_int xmin, ti_int ymin, ti_int xmax, ti_int ymax) {
    lib.gfz_SetClipRegion(xmin, ymin, xmax, ymax);
}

bool gfy_GetClipRegion(gfy_region_t *region) {
    return lib.gfz_GetClipRegion(reinterpret_cast<gfz_region_t*>(region));
}

void gfy_ShiftDown(uint8_t pixels) {
    lib.gfz_ShiftDown(pixels);
}

void gfy_ShiftUp(uint8_t pixels) {
    lib.gfz_ShiftUp(pixels);
}

void gfy_ShiftLeft(ti_uint pixels) {
    lib.gfz_ShiftLeft(pixels);
}

void gfy_ShiftRight(ti_uint pixels) {
    lib.gfz_ShiftRight(pixels);
}

uint16_t gfy_Lighten(uint16_t color, uint8_t amount) {
    return lib.gfz_Lighten(color, amount);
}

uint16_t gfy_Darken(uint16_t color, uint8_t amount) {
    return lib.gfz_Darken(color, amount);
}

void gfy_FloodFill(ti_uint x, uint8_t y, uint8_t color) {
    lib.gfz_FloodFill(x, y, color);
}

void gfy_RLETSprite(
    const gfy_rletsprite_t *sprite,
    ti_int x, ti_int y
) {
    lib.gfz_RLETSprite(
        reinterpret_cast<const gfz_rletsprite_t*>(sprite),
        x, y
    );
}

void gfy_RLETSprite_NoClip(
    const gfy_rletsprite_t *sprite,
    ti_uint x, uint8_t y
) {
    lib.gfz_RLETSprite_NoClip(
        reinterpret_cast<const gfz_rletsprite_t*>(sprite),
        x, y
    );
}

gfy_sprite_t *gfy_ConvertFromRLETSprite(
    const gfy_rletsprite_t *sprite_in,
    gfy_sprite_t *sprite_out
) {
    return reinterpret_cast<gfy_sprite_t*>(
        lib.gfz_ConvertFromRLETSprite(
            reinterpret_cast<const gfz_rletsprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_sprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_rletsprite_t *gfy_ConvertToRLETSprite(
    const gfy_sprite_t *sprite_in,
    gfy_rletsprite_t *sprite_out
) {
    return reinterpret_cast<gfy_rletsprite_t*>(
        lib.gfz_ConvertToRLETSprite(
            reinterpret_cast<const gfz_sprite_t *__restrict>(sprite_in),
            reinterpret_cast<gfz_rletsprite_t *__restrict>(sprite_out)
        )
    );
}

gfy_rletsprite_t *gfy_ConvertToNewRLETSprite(
    const gfy_sprite_t *sprite_in,
    void *(*malloc_routine)(size_t)
) {
    return reinterpret_cast<gfy_rletsprite_t*>(
        lib.gfz_ConvertToNewRLETSprite(
            reinterpret_cast<const gfz_sprite_t*>(sprite_in),
            malloc_routine
        )
    );
}

#endif /* LIB_GRAPHY_HPP */
