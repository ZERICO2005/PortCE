#ifndef FRAME_MANIPULATION_HPP
#define FRAME_MANIPULATION_HPP

#include <cstdint>
#include <cstring>

constexpr size_t VIDEO_CHANNELS = 4;

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

inline uint32_t bgr888_to_abgr8888(uint8_t b, uint8_t g, uint8_t r) {
    constexpr uint8_t alpha = UINT8_C(0xFF);
    uint32_t ret = 0;
    ret |= (static_cast<uint32_t>(r    ) <<  0);
    ret |= (static_cast<uint32_t>(g    ) <<  8);
    ret |= (static_cast<uint32_t>(b    ) << 16);
    ret |= (static_cast<uint32_t>(alpha) << 24);
    return ret;
}

inline uint32_t rgb888_to_abgr8888(uint8_t b, uint8_t g, uint8_t r) {
    constexpr uint8_t alpha = UINT8_C(0xFF);
    uint32_t ret = 0;
    ret |= (static_cast<uint32_t>(b    ) <<  0);
    ret |= (static_cast<uint32_t>(g    ) <<  8);
    ret |= (static_cast<uint32_t>(r    ) << 16);
    ret |= (static_cast<uint32_t>(alpha) << 24);
    return ret;
}

inline uint32_t bgr24_to_abgr8888(uint32_t color) {
    uint8_t r = static_cast<uint8_t>(color >>  0);
    uint8_t g = static_cast<uint8_t>(color >>  8);
    uint8_t b = static_cast<uint8_t>(color >> 16);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t rgb24_to_abgr888(uint32_t color) {
    uint8_t b = static_cast<uint8_t>(color >>  0);
    uint8_t g = static_cast<uint8_t>(color >>  8);
    uint8_t r = static_cast<uint8_t>(color >> 16);
    return rgb888_to_abgr8888(b, g, r);
}

inline void rgb1555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    constexpr int r_comp = 5;
    constexpr int g_comp = 5;
    constexpr int b_comp = 5;
    constexpr int a_comp = 1;
    constexpr int r_shift = r_comp;
    constexpr int g_shift = g_comp + a_comp;
    constexpr int b_shift = b_comp;
    union {
        uint16_t bin;
        struct {
            uint16_t r : r_comp;
            uint16_t g : g_comp;
            uint16_t b : b_comp;
            uint16_t a : a_comp;
        } comp;
    } c;
    c.bin = color;
    r = static_cast<uint8_t>(c.comp.r);
    g = static_cast<uint8_t>((c.comp.g << 1) | (c.comp.a));
    b = static_cast<uint8_t>(c.comp.b);
    r <<= (8 - r_shift);
    g <<= (8 - g_shift);
    b <<= (8 - b_shift);
    r += (r >> r_shift);
    g += (g >> g_shift);
    b += (b >> b_shift);
}

inline void rgb555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    constexpr int r_comp = 5;
    constexpr int g_comp = 5;
    constexpr int b_comp = 5;
    constexpr int r_shift = r_comp;
    constexpr int g_shift = g_comp;
    constexpr int b_shift = b_comp;
    union {
        uint16_t bin;
        struct {
            uint16_t r : r_comp;
            uint16_t g : g_comp;
            uint16_t b : b_comp;
        } comp;
    } c;
    c.bin = color;
    r = static_cast<uint8_t>(c.comp.r);
    g = static_cast<uint8_t>(c.comp.g);
    b = static_cast<uint8_t>(c.comp.b);
    r <<= (8 - r_shift);
    g <<= (8 - g_shift);
    b <<= (8 - b_shift);
    r += (r >> r_shift);
    g += (g >> g_shift);
    b += (b >> b_shift);
}

inline void rgb565_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    constexpr int r_comp = 5;
    constexpr int g_comp = 5;
    constexpr int b_comp = 5;
    constexpr int r_shift = r_comp;
    constexpr int g_shift = g_comp;
    constexpr int b_shift = b_comp;
    union {
        uint16_t bin;
        struct {
            uint16_t r : r_comp;
            uint16_t g : g_comp;
            uint16_t b : b_comp;
        } comp;
    } c;
    c.bin = color;
    r = static_cast<uint8_t>(c.comp.r);
    g = static_cast<uint8_t>(c.comp.g);
    b = static_cast<uint8_t>(c.comp.b);
    r <<= (8 - r_shift);
    g <<= (8 - g_shift);
    b <<= (8 - b_shift);
    r += (r >> r_shift);
    g += (g >> g_shift);
    b += (b >> b_shift);
}

inline void rgb444_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    constexpr int r_comp = 4;
    constexpr int g_comp = 4;
    constexpr int b_comp = 4;
    constexpr int r_shift = r_comp;
    constexpr int g_shift = g_comp;
    constexpr int b_shift = b_comp;
    union {
        uint16_t bin;
        struct {
            uint16_t r : r_comp;
            uint16_t g : g_comp;
            uint16_t b : b_comp;
        } comp;
    } c;
    c.bin = color;
    r = static_cast<uint8_t>(c.comp.r);
    g = static_cast<uint8_t>(c.comp.g);
    b = static_cast<uint8_t>(c.comp.b);
    r <<= (8 - r_shift);
    g <<= (8 - g_shift);
    b <<= (8 - b_shift);
    r += (r >> r_shift);
    g += (g >> g_shift);
    b += (b >> b_shift);
}

inline void bgr1555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb1555_to_bgr888(color, r, g, b);
}

inline void bgr555_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb555_to_bgr888(color, r, g, b);
}

inline void bgr565_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb565_to_bgr888(color, r, g, b);
}

inline void bgr444_to_bgr888(uint16_t color, uint8_t& b, uint8_t& g, uint8_t& r) {
    rgb444_to_bgr888(color, r, g, b);
}

inline uint32_t bgr1555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr1555_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t bgr555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr555_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t bgr565_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr565_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t bgr444_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    bgr444_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t rgb1555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    rgb1555_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t rgb555_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    rgb555_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t rgb565_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    rgb565_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

inline uint32_t rgb444_to_abgr8888(uint16_t color) {
    uint8_t b, g, r;
    rgb444_to_bgr888(color, b, g, r);
    return bgr888_to_abgr8888(b, g, r);
}

#endif /* FRAME_MANIPULATION_HPP */
