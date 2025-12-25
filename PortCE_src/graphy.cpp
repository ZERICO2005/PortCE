#include "lib_graphy.hpp"

template<>
void GraphY::gfz_SetPixel_NoClip(uint32_t x, uint8_t y, uint8_t color) const {
    if (x < GFY_LCD_WIDTH && y < GFY_LCD_HEIGHT) {
        ((uint8_t*)RAM_ADDRESS(CurrentBuffer))[(uint24_t)x + (y * GFY_LCD_WIDTH)] = color;
    }
}
