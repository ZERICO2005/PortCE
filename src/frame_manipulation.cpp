#include "frame_manipulation.hpp"

#include <cstdio>

//------------------------------------------------------------------------------
// blitting
//------------------------------------------------------------------------------

static void blit16bpp_row_major(
    uint32_t *__restrict dst,
    uint16_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    const size_t pixel_count = width * height;
    for (size_t i = 0; i < pixel_count; i++) {
        *dst++ = palette[*src++];
    }
}

static void blit16bpp_column_major(
    uint32_t *__restrict dst,
    uint16_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    const size_t jump = (width * height) - 1;
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            *dst++ = palette[*src];
            src += height;
        }
        src -= jump;
    }
}

static void blit8bpp_row_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    const size_t pixel_count = width * height;
    for (size_t i = 0; i < pixel_count; i++) {
        *dst++ = palette[*src++];
    }
}

static void blit8bpp_column_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    const size_t jump = (width * height) - 1;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            *dst++ = palette[*src];
            src += height;
        }
        src -= jump;
    }
}

static void blit4bpp_row_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    constexpr uint8_t PixelsPerByte = 2;
    size_t w = 0;
    size_t z = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width / PixelsPerByte; x++) {
            uint8_t c = src[z];
            dst[w] = palette[c & 0xF]; w++;
            dst[w] = palette[(c >> 4) & 0xF]; w++;
            z++;
        }
    }
}

static void blit4bpp_column_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    constexpr uint8_t PixelsPerByte = 2;
    size_t w = 0;
    size_t z = 0;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y += PixelsPerByte) {
            uint8_t c = src[z];
            for (uint8_t i = 0; i < PixelsPerByte; i++) {
                dst[w] = palette[c & 0xF]; w++;
                w += (width - 1);
                c >>= 4;
            }
            z++;
        }
        w -= width * height;
        w += 1;
    }
}

static void blit2bpp_row_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    constexpr uint8_t PixelsPerByte = 4;
    size_t w = 0;
    size_t z = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width / PixelsPerByte; x++) {
            uint8_t c = src[z];
            dst[w] = palette[(c >> 0) & 0x3]; w++;
            dst[w] = palette[(c >> 2) & 0x3]; w++;
            dst[w] = palette[(c >> 4) & 0x3]; w++;
            dst[w] = palette[(c >> 6) & 0x3]; w++;
            z++;
        }
    }
}

static void blit2bpp_column_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    constexpr uint8_t PixelsPerByte = 4;
    size_t w = 0;
    size_t z = 0;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y += PixelsPerByte) {
            uint8_t c = src[z];
            for (uint8_t i = 0; i < PixelsPerByte; i++) {
                dst[w] = palette[c & 0x3]; w++;
                w += (width - 1);
                c >>= 2;
            }
            z++;
        }
        w -= width * height;
        w += 1;
    }
}

static void blit1bpp_row_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    constexpr uint8_t PixelsPerByte = 8;
    size_t w = 0;
    size_t z = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width / PixelsPerByte; x++) {
            uint8_t c = src[z];
            for (uint8_t b = 0; b < PixelsPerByte; b++) {
                dst[w] = palette[(c >> b) & 0x1]; w++;
            }
            z++;
        }
    }
}

static void blit1bpp_column_major(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height
) {
    constexpr uint8_t PixelsPerByte = 8;
    size_t w = 0;
    size_t z = 0;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y += PixelsPerByte) {
            uint8_t c = src[z];
            for (uint8_t i = 0; i < PixelsPerByte; i++) {
                dst[w] = palette[c & 0x1]; w++;
                w += (width - 1);
                c >>= 1;
            }
            z++;
        }
        w -= width * height;
        w += 1;
    }
}

//------------------------------------------------------------------------------

static void blit16bpp(
    uint32_t *__restrict dst,
    uint16_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height,
    bool column_major
) {
    if (column_major) {
        blit16bpp_column_major(dst, src, palette, width, height);
        return;
    }
    blit16bpp_row_major(dst, src, palette, width, height);
}

static void blit8bpp(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height,
    bool column_major
) {
    if (column_major) {
        blit8bpp_column_major(dst, src, palette, width, height);
        return;
    }
    blit8bpp_row_major(dst, src, palette, width, height);
}

static void blit4bpp(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height,
    bool column_major
) {
    if (column_major) {
        blit4bpp_column_major(dst, src, palette, width, height);
        return;
    }
    blit4bpp_row_major(dst, src, palette, width, height);
}

static void blit2bpp(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height,
    bool column_major
) {
    if (column_major) {
        blit2bpp_column_major(dst, src, palette, width, height);
        return;
    }
    blit2bpp_row_major(dst, src, palette, width, height);
}

static void blit1bpp(
    uint32_t *__restrict dst,
    uint8_t const *__restrict src,
    uint32_t const *__restrict palette,
    size_t width,
    size_t height,
    bool column_major
) {
    if (column_major) {
        blit1bpp_column_major(dst, src, palette, width, height);
        return;
    }
    blit1bpp_row_major(dst, src, palette, width, height);
}

//------------------------------------------------------------------------------
// selection
//------------------------------------------------------------------------------

static uint32_t PreCalc_RGB1555[65536];
static uint32_t PreCalc_BGR1555[65536];
static uint32_t PreCalc_RGB565[65536];
static uint32_t PreCalc_BGR565[65536];
static uint32_t PreCalc_RGB555[65536];
static uint32_t PreCalc_BGR555[65536];
static uint32_t PreCalc_RGB444[65536];
static uint32_t PreCalc_BGR444[65536];
static uint32_t PaletteRam[65536];

static void init_palette() {
    static bool init = false;
    if (init) {
        return;
    }
    init = true;

    for (size_t i = 0; i < 65536; i++) {
        uint16_t c = (uint16_t)i;
        PreCalc_RGB1555[i] = rgb1555_to_abgr8888(c);
        PreCalc_BGR1555[i] = bgr1555_to_abgr8888(c);
        PreCalc_RGB565[i] = rgb565_to_abgr8888(c);
        PreCalc_BGR565[i] = bgr565_to_abgr8888(c);
        PreCalc_RGB555[i] = rgb555_to_abgr8888(c);
        PreCalc_BGR555[i] = bgr555_to_abgr8888(c);
        PreCalc_RGB444[i] = rgb444_to_abgr8888(c);
        PreCalc_BGR444[i] = bgr444_to_abgr8888(c);
    }
}

static void calculate_palette(const Frame_Manipulation& data) {
    init_palette();
    switch (data.color_mode) {
        case Color_Mode::Indexed_1:
        case Color_Mode::Indexed_2:
        case Color_Mode::Indexed_4:
        case Color_Mode::Indexed_8:
        {
            for (size_t i = 0; i < 256; i++) {
                uint16_t c = data.palette[i];
                PaletteRam[i] = data.bgr ? bgr1555_to_abgr8888(c) : rgb1555_to_abgr8888(c);
            }
        } break;
        case Color_Mode::Color_1555:
        {
            memcpy(PaletteRam, data.bgr ? PreCalc_BGR1555 : PreCalc_RGB1555, sizeof(PaletteRam));
        } break;
        case Color_Mode::Color_565:
        {
            memcpy(PaletteRam, data.bgr ? PreCalc_BGR565 : PreCalc_RGB565, sizeof(PaletteRam));
        } break;
        case Color_Mode::Color_555:
        {
            memcpy(PaletteRam, data.bgr ? PreCalc_BGR555 : PreCalc_RGB555, sizeof(PaletteRam));
        } break;
        case Color_Mode::Color_444:
        {
            memcpy(PaletteRam, data.bgr ? PreCalc_BGR444 : PreCalc_RGB444, sizeof(PaletteRam));
        } break;
    }
    if (data.invert_colors) {
        for (size_t i = 0; i < 65536; i++) {
            PaletteRam[i] ^= 0x00FFFFFF;
        }
    }
    if (data.idle_mode) {
        for (size_t i = 0; i < 65536; i++) {
            PaletteRam[i] &= 0xFF808080;
        }
    }
}

void frame_copy(const Frame_Manipulation& data) {
    if (!data.is_valid_state()) {
        return;
    }
    calculate_palette(data);
    bool column_major = data.transpose;
    size_t width = data.get_width();
    size_t height = data.get_height();

    switch (data.color_mode) {
        case Color_Mode::Indexed_1  : blit1bpp (data.dst, data.get_src8() , PaletteRam, width, height, column_major); break;
        case Color_Mode::Indexed_2  : blit2bpp (data.dst, data.get_src8() , PaletteRam, width, height, column_major); break;
        case Color_Mode::Indexed_4  : blit4bpp (data.dst, data.get_src8() , PaletteRam, width, height, column_major); break;
        case Color_Mode::Indexed_8  : blit8bpp (data.dst, data.get_src8() , PaletteRam, width, height, column_major); break;
        case Color_Mode::Color_1555 : blit16bpp(data.dst, data.get_src16(), PaletteRam, width, height, column_major); break;
        case Color_Mode::Color_565  : blit16bpp(data.dst, data.get_src16(), PaletteRam, width, height, column_major); break;
        case Color_Mode::Color_555  : blit16bpp(data.dst, data.get_src16(), PaletteRam, width, height, column_major); break;
        case Color_Mode::Color_444  : blit16bpp(data.dst, data.get_src16(), PaletteRam, width, height, column_major); break;
    }
}
