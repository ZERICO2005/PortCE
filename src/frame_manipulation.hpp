#ifndef FRAME_MANIPULATION_HPP
#define FRAME_MANIPULATION_HPP

#include <cstdint>
#include <cstring>

enum class Color_Mode {
    Indexed_1,
    Indexed_2,
    Indexed_4,
    Indexed_8,
    Color_1555,
    Color_565,
    Color_555,
    Color_444,
};

constexpr size_t bits_per_pixel(Color_Mode color) {
    switch(color) {
        case Color_Mode::Indexed_1:
            return 1;
        case Color_Mode::Indexed_2:
            return 2;
        case Color_Mode::Indexed_4:
            return 4;
        case Color_Mode::Indexed_8:
            return 8;
        case Color_Mode::Color_1555:
        case Color_Mode::Color_565:
        case Color_Mode::Color_555:
        case Color_Mode::Color_444:
            return 16;
    }
}

struct Frame_Manipulation {
    void const *__restrict src;
    uint16_t const *__restrict palette;
    uint32_t *__restrict dst;
    bool transpose;
    bool bgr;
    bool invert_colors;
    bool idle_mode;
    Color_Mode color_mode;
    int width;
    int height;

    bool is_valid_state() const {
        if (src == nullptr || dst == nullptr) {
            return false;
        }
        if (width <= 0 || height <= 0) {
            return false;
        }
        /**
         * Not sure how to display 1 bit per pixel images if the resolution
         * is not a multiple of 8.
         */
        if (width % 8 != 0 || height % 8 != 0) {
            return false;
        }
        return true;
    }

    size_t pixel_count() const {
        return static_cast<size_t>(width * height);
    }
    size_t get_width() const {
        return static_cast<size_t>(width);
    }
    size_t get_height() const {
        return static_cast<size_t>(height);
    }
    size_t bits_per_pixel() const {
        return ::bits_per_pixel(color_mode);
    }

    uint16_t const *get_src16() const {
        return static_cast<uint16_t const *__restrict>(src);
    }
    uint8_t const *get_src8() const {
        return static_cast<uint8_t const *__restrict>(src);
    }
};

void frame_copy(const Frame_Manipulation& data);

inline void rgb1555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    r = (uint8_t)(color & 0x1F);
    g = (uint8_t)((color & 0x3E0) >> 4) + ((color & 0x8000) ? 1 : 0);
    b = (uint8_t)((color & 0x7C00) >> 10);
    r *= 8; g *= 4; b *= 8;
    r += r / 32;
    g += g / 64;
    b += b / 32;
}

inline void rgb555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    r = (uint8_t)(color & 0x1F);
    g = (uint8_t)((color & 0x3E0) >> 5);
    b = (uint8_t)((color & 0x7C00) >> 10);
    r *= 8; g *= 8; b *= 8;
    r += r / 32;
    g += g / 32;
    b += b / 32;
}

inline void rgb565_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    union {
        uint16_t bin;
        struct {
            uint16_t r : 5;
            uint16_t g : 6;
            uint16_t b : 5;
        } comp;
    } c;
    c.bin = color;
    r = c.comp.r;
    g = c.comp.g;
    b = c.comp.b;
    r *= 8; g *= 4; b *= 8;
    r += r / 32;
    g += g / 64;
    b += b / 32;
}

inline void rgb444_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    r = (uint8_t)(color & 0xF);
    g = (uint8_t)((color & 0xF0) >> 4);
    b = (uint8_t)((color & 0xF00) >> 8);
    r *= 16; g *= 16; b *= 16;
    r += r / 16;
    g += g / 16;
    b += b / 16;
}

inline void bgr1555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb1555_to_bgr888(color, r, g, b);
}

inline void bgr555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb555_to_bgr888(color, r, g, b);
}

inline void bgr565_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb565_to_bgr888 (color, r, g, b);
}

inline void bgr444_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb444_to_bgr888(color, r, g, b);
}

inline uint32_t bgr1555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr1555_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t bgr555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr555_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t bgr565_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr565_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t bgr444_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr444_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t rgb1555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr1555_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t rgb555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr555_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t rgb565_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr565_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

inline uint32_t rgb444_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr444_to_bgr888(color, b, g, r);
    return (UINT32_C(0xFF) << 24) | (b << 16) | (g << 8) | (r << 0);
}

#endif /* FRAME_MANIPULATION_HPP */
