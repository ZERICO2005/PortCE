/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "../PortCE_Common.h"
#include "copyBuffer.h"
#include "bitGraphics.h"
#include "../bitGraphics_Font/bitGraphics_Font.h"
#include "color.hpp"

constexpr size_t Bit_Graphics_Channels = 4;

extern const uint8_t char6x8[1536]; // bitGraphics_Font.h

Bit_Graphics::Bit_Graphics() {
	buf = nullptr;
	buf0 = nullptr;
	buf1 = nullptr;
	initialized = false;
	terminate_Bit_Graphics();
}
Bit_Graphics::Bit_Graphics(size_t resX, size_t resY) {
	buf = nullptr;
	buf0 = nullptr;
	buf1 = nullptr;
	initialized = false;
	init_Bit_Graphics(resX,resY);
}

void Bit_Graphics::init_Bit_Graphics(size_t resX, size_t resY) {
	if (initialized == true) { return; }
	if (resX == 0 || resY == 0) { return; }
	ResX = resX;
	ResY = resY;
	const size_t pixel_count = resX * resY;
	buf0 = (uint32_t*)calloc(pixel_count, sizeof(uint32_t));
	if (buf0 == nullptr) { terminate_Bit_Graphics(); return; }
	buf1 = (uint32_t*)calloc(pixel_count, sizeof(uint32_t));
	if (buf1 == nullptr) { terminate_Bit_Graphics(); return; }
	buf = buf1;
	gColor_RGB(0,0,0);
	initialized = true;
	buf = buf0;
	clearBuffer();
	swapBuffer();
	clearBuffer();
}
void Bit_Graphics::terminate_Bit_Graphics() {
	initialized = false;
	buf = nullptr;
	FREE(buf0);
	FREE(buf1);
	ResX = 0;
	ResY = 0;
	gColor_RGB(0,0,0);
}
void Bit_Graphics::resizeBuffer(size_t resX, size_t resY) {
	if (initialized == false) { return; }
	if (resX == ResX && resY == ResY) { return; } // No changes made
	if (resX == 0 || resY == 0) { return; }
	const size_t prev_pixel_count = getPixelCount();
	ResX = resX;
	ResY = resY;
	if (getPixelCount() != prev_pixel_count) { // Different amount of bytes allocated
		buf0 = (uint32_t*)realloc(buf0, getBufferSize());
		if (buf0 == nullptr) { terminate_Bit_Graphics(); return; }
		buf1 = (uint32_t*)realloc(buf1, getBufferSize());
		if (buf1 == nullptr) { terminate_Bit_Graphics(); return; }
	}
	buf = buf0;
	clearBuffer();
	swapBuffer();
	clearBuffer();
}
void Bit_Graphics::swapBuffer() {
	if (initialized == false) { return; }
	buf = (buf == buf0) ? buf1 : buf0;
}
void Bit_Graphics::clearBuffer() {
	if (initialized == false) { return; }
	memset(buf, 0, getBufferSize());
}
uint32_t* Bit_Graphics::getDrawBuffer() const {
	if (initialized == false) { return nullptr; }
	return (buf == buf0) ? buf0 : buf1;
}
uint32_t* Bit_Graphics::getDisplayBuffer() const {
	if (initialized == false) { return nullptr; }
	return (buf == buf0) ? buf1 : buf0;
}
void Bit_Graphics::getDrawBufferBox(BufferBox* box) {
	if (box == nullptr) { return; }
	if (initialized == false) {
		box->vram = nullptr;
		box->resX = 0;
		box->resY = 0;
		box->channels = 0;
		box->padding = 0;
	}
	box->vram = reinterpret_cast<uint8_t*>((buf == buf0) ? buf0 : buf1);
	box->resX = (int)ResX;
	box->resY = (int)ResY;
	box->channels = Bit_Graphics_Channels;
	box->padding = 0;
}
void Bit_Graphics::getDisplayBufferBox(BufferBox* box) {
	if (box == nullptr) { return; }
	if (initialized == false) {
		box->vram = nullptr;
		box->resX = 0;
		box->resY = 0;
		box->channels = 0;
		box->padding = 0;
	}
	box->vram = reinterpret_cast<uint8_t*>((buf == buf0) ? buf1 : buf0);
	box->resX = (int)ResX;
	box->resY = (int)ResY;
	box->channels = Bit_Graphics_Channels;
	box->padding = 0;
}
/* Set color */
void Bit_Graphics::gColor_HSV(double hue, double sat, double val) { // 0.0-360.0, 0.0-1.0, 0.0-1.0
	uint8_t* const gColor8 = reinterpret_cast<uint8_t*>(&gColor);
	getRGBfromHSV(&gColor8[0], &gColor8[1], &gColor8[2], hue, sat, val);
	gColor8[3] = 0xFF;
}
void Bit_Graphics::gColor_HSV(float hue, float sat, float val) { // 0.0-360.0, 0.0-1.0, 0.0-1.0
	uint8_t* const gColor8 = reinterpret_cast<uint8_t*>(&gColor);
	getRGBfromHSV(&gColor8[0], &gColor8[1], &gColor8[2], hue, sat, val);
	gColor8[3] = 0xFF;
}

/* Internal Routines | No safety checks */
inline void Bit_Graphics::internal_plot(size_t x, size_t y) {
	uint32_t* fill = buf + (y * ResX + x);
	*fill = gColor;
}
inline void Bit_Graphics::internal_plotFast(size_t z) { //z position
	uint32_t* fill = buf + z;
	*fill = gColor;
}
inline void Bit_Graphics::internal_horiz(size_t x0, size_t y, size_t x1) { //x start, y postion, x length
	uint32_t* fill = buf + (x0 + y * ResX);
	patternMemcpy(fill, x1 * sizeof(uint32_t), &gColor, sizeof(uint32_t));
}
/* Primatives */
void Bit_Graphics::plot(size_t x, size_t y) { //x position, y position
	if (x >= ResX || y >= ResY) { return; }
	uint32_t* fill = buf + (y * ResX + x);
	*fill = gColor;
}
void Bit_Graphics::plotFast(size_t z) { //z position
	if (z > getPixelCount()) { return; }
	uint32_t* fill = buf + z;
	*fill = gColor;
}
void Bit_Graphics::fillScreen() {
	patternMemcpy(buf, getBufferSize(), &gColor, sizeof(uint32_t));
}
void Bit_Graphics::fillRect(size_t x0, size_t y0, size_t x1, size_t y1) { //x start, y start, x length, y length
	if (x0 >= ResX || y0 >= ResY || x1 == 0 || y1 == 0) { return; }
	if (x0 + x1 > ResX || y0 + y1 > ResY) { return; }
	uint32_t* fill = buf + (y0 * ResX + x0);
	for (size_t dY = 0; dY < y1; dY++) {
		patternMemcpy(fill, x1 * sizeof(uint32_t), &gColor, sizeof(uint32_t));
		fill += ResX;
	}
}
void Bit_Graphics::horiz(size_t x0, size_t y, size_t x1) { //x start, y postion, x length
	if (x0 >= ResX || y >= ResY || x1 == 0) { return; }
	if (x0 + x1 > ResX) { return; }
	uint32_t* fill = buf + (x0 + y * ResX);
	patternMemcpy(fill, x1 * sizeof(uint32_t), &gColor, sizeof(uint32_t));
}
void Bit_Graphics::vert(size_t x, size_t y0, size_t y1) { //x postion, y start, y length
	if (x >= ResX || y0 >= ResY || y1 == 0) { return; }
	if (y0 + y1 > ResY) { return; }
	uint32_t* fill = buf + x + y0 * ResX;
	for (size_t i = 0; i < y1; i++) {
		*fill = gColor;
		fill += ResX;
	}
}
void Bit_Graphics::drawRect(size_t x0, size_t y0, size_t x1, size_t y1) { //x start, y start, x length, y length
	horiz(x0,y0,x1);
	horiz(x0,y0+y1-1,x1);
	vert(x0,y0,y1);
	vert(x0+x1-1,y0,y1);
}

void Bit_Graphics::drawLine0(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
	int32_t dX = x1 - x0;
	int32_t dY = y1 - y0;
	int32_t yI = 1;
	if (dY < 0) {
		yI = -1;
		dY = -dY;
	}
	int32_t dD = 2 * dY - dX;
	int32_t y = y0;
	for (int32_t x = x0; x < x1; x++) {
		plot((size_t)x, (size_t)y);
		if (dD > 0) {
			y += yI;
			dD += 2 * (dY - dX);
		} else {
			dD += 2 * dY;
		}
	}
}

void Bit_Graphics::drawLine1(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
	int32_t dX = x1 - x0;
	int32_t dY = y1 - y0;
    int32_t xI = 1;
    if (dX < 0) {
		xI = -1;
		dX = -dX;
	}
    int32_t dD = (2 * dX) - dY;
    int32_t x = x0;

    for (int32_t y = y0; y < y1; y++) {
        plot((size_t)x, (size_t)y);
        if (dD > 0) {
            x = x + xI;
            dD = dD + (2 * (dX - dY));
        } else {
            dD = dD + 2 * dX;
        }
	}
}

void Bit_Graphics::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLine0(x1, y1, x0, y0);
		} else {
            drawLine0(x0, y0, x1, y1);
		}
    } else {
        if (y0 > y1) {
            drawLine1(x1, y1, x0, y0);
        } else {
            drawLine1(x0, y0, x1, y1);
		}
    }
}

/* Text */

/* const uint8_t char6x8[1536] can be found in graphics2D_Font.h */

void Bit_Graphics::text6x8(size_t xW, size_t yW, char lexicon) {
	if (xW > ResX || yW > ResY) { return; }
	if (xW + 6 > ResX || yW + 8 > ResY) { return; }
	const uint8_t* bitImage = (const uint8_t*)char6x8 + ((size_t)((uint8_t)lexicon) * 6);
	uint32_t* fill = buf + (yW * ResX + xW);
	uint8_t b = 1;
	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 6; x++) {
			if (*bitImage & b) {
				*fill = gColor;
			}
			bitImage++;
			fill++;
		}
		bitImage -= 6;
		fill += ResX - 6; // 320 - 6
		b <<= 1;
	}
}
void Bit_Graphics::printText6x8(size_t xW, size_t yW, const char* text) {
	if (initialized == false) { return; }
	size_t x = xW;
	size_t y = yW;
	size_t tabPos = 0; // For tabs
	while (*text != '\0') {
		switch(*text) {
			case '\n': // New Line
				x = xW;
				y += 9;
				tabPos = 0;
				break;
			case '\t': // Horizontal Tab
				x += (4 - ((tabPos - 1) % 4)) * 7;
				tabPos = 0;
				break;
			case '\v': // Vertical Tab
				y += 9;
				break;
			case '\b': // Backspace
				x -= 7;
				break;
			case '\r': // Return
				x = xW;
				break;
			default: //Text
				text6x8(x,y,*text);
				x += 7;
		}
		text++;
		tabPos++;
	}
}
void Bit_Graphics::printTextWarp6x8(size_t xW, size_t yW, const char* text, size_t width = 0) {
	if (initialized == false) { return; }
	if (width == 0) { width = ResX; }
	size_t x = xW;
	size_t y = yW;
	size_t tabPos = 0; // For tabs
	while (*text != '\0') {
		switch(*text) {
			case '\n': // New Line
				x = xW;
				y += 9;
				tabPos = 0;
				break;
			case '\t': // Horizontal Tab
				x += (4 - ((tabPos - 1) % 4)) * 7;
				tabPos = 0;
				break;
			case '\v': // Vertical Tab
				y += 9;
				break;
			case '\b': // Backspace
				x -= 7;
				break;
			case '\r': // Return
				x = xW;
				break;
			default: //Text
				text6x8(x,y,*text);
				x += 7;
			if (xW + 7 > width) { // New Line
				x = xW;
				y += 9;
				tabPos = 0;
			}
		}
		text++;
		tabPos++;
	}
}
/*
void Bit_Graphics::debugPrintBuffer() {
	gColor_RGB(48,8,48);
	fillRect(12,12,29,10);
	gColor_RGB(216,255,216);
	if (buf == buf0) {
		printText6x8(13,13,"buf0");
	} else if (buf == buf1) {
		printText6x8(13,13,"buf1");
	} else {
		printText6x8(13,13,"buf?");
	}
}
*/
