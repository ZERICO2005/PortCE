/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef KEYBIND_H
#define KEYBIND_H

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

/* 0 = ANSI 104key, 1 = Extended 152key, 2 = Complete 242key */
void renderKeyboard(
	Render* buf, int32_t ResX, double minScaleX, double maxScaleX,
	uint8_t KeyboardSize, bool includeNumpad,
	int32_t curX, int32_t curY, bool click, SDL_Scancode* KB_scancode, SDL_KeyCode* KB_keycode, bool* hoverInBounds
);

// Default colors
void initKeyboardGraphics(double hue, double sat, double val);
void terminateKeyboardGraphics();

void setRGB_Scancode(uint8_t r, uint8_t g, uint8_t b, SDL_Scancode code);
void setColor_Scancode(uint32_t color, SDL_Scancode code);
void setHSV_Scancode(double h, double s, double v, SDL_Scancode code);

#endif /* KEYBIND_H */
