/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef KEYBIND_H
#define KEYBIND_H

#include "../PortCE_Common.h"

#include "fracExpKB.h"
#include "copyBuffer.h"
#include "color.hpp"
#include <SDL_keycode.h>
#include <SDL_scancode.h>
#include <list>

constexpr size_t IMAGE_BUFFER_CHANNELS = 4;


extern size_t KeyBind_PresetCount;
extern std::list<KeyBind_Preset> KeyBind_PresetList;
extern KeyBind_Preset* currentKBPreset;


/* 0 = ANSI 104key, 1 = Extended 152key, 2 = Complete 242key */
void renderKeyboard(
	BufferBox* buf, int32_t ResX, double minScaleX, double maxScaleX,
	uint8_t KeyboardSize, bool includeNumpad,
	int32_t curX, int32_t curY, bool click, SDL_Scancode* KB_scancode, SDL_KeyCode* KB_keycode, bool* hoverInBounds
);

// Default colors
void initKeyboardGraphics(double hue, double sat, double val);
void terminateKeyboardGraphics();

void setRGB_Scancode(uint8_t r, uint8_t g, uint8_t b, SDL_Scancode code);
void setColor_Scancode(uint32_t color, SDL_Scancode code);
void setHSV_Scancode(double h, double s, double v, SDL_Scancode code);

void updateKeys();
void recolorKeyboard();
void initKeys();
bool keyPressed(SDL_Scancode key);
int setup_fracExpKB(int argc, char* argv[]);

void init_KeyBind_PresetList();
void clear_KeyBind_PresetList();
int32_t get_currentKBPreset_Pos();
void set_currentKBPreset_Pos(int pos);
void remove_currentKBPreset();

#endif /* KEYBIND_H */
