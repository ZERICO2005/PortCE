/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef BITGRAPHICS_H
#define BITGRAPHICS_H

#include <cstdint>
#include <cstring>

struct BufferBox;

class Bit_Graphics {
	public:
		Bit_Graphics();
		Bit_Graphics(size_t resX, size_t resY);

		bool isInitialized() const { return initialized; }
		void init_Bit_Graphics(size_t resX, size_t resY);
		void terminate_Bit_Graphics();
		void resizeBuffer(size_t resX, size_t resY);
		void swapBuffer();
		/** @brief memset the draw-buffer to zero */
		void clearBuffer();
		uint32_t* getDrawBuffer() const;
		uint32_t* getDisplayBuffer() const;
		void getDrawBufferBox(BufferBox* box);
		void getDisplayBufferBox(BufferBox* box);

		inline size_t getResX() const { return ResX; }
		inline size_t getResY() const { return ResY; }
		inline size_t getPixelCount() const { return ResX * ResY; }
		inline size_t getBufferSize() const { return ResX * ResY * sizeof(uint32_t); }

	/* Set color */
		inline void gColor_RGB(uint8_t r, uint8_t g, uint8_t b) {
			uint8_t* const gColor8 = reinterpret_cast<uint8_t*>(&gColor);
			gColor8[0] = r;
			gColor8[1] = g;
			gColor8[2] = b;
			gColor8[3] = 0xFF;
		}
		inline void gColor_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			uint8_t* const gColor8 = reinterpret_cast<uint8_t*>(&gColor);
			gColor8[0] = r;
			gColor8[1] = g;
			gColor8[2] = b;
			gColor8[3] = a;
		}
		inline void gColor_RGB_hex(uint32_t col) {
			gColor = col | UINT32_C(0xFF000000);
		};
		inline void gColor_RGBA_hex(uint32_t col) {
			gColor = col;
		};
		void gColor_HSV(double h, double s, double v); // 0.0-360.0, 0.0-1.0, 0.0-1.0
		void gColor_HSV(float h, float s, float v); // 0.0-360.0, 0.0-1.0, 0.0-1.0
	/* Primatives */
		void plot(size_t x, size_t y); //x position, y position
		void plotFast(size_t z); //z position
		void fillScreen();
		void fillRect(size_t x0, size_t y0, size_t x1, size_t y1); //x start, y start, x length, y length
		void horiz(size_t x0, size_t y, size_t x1); //x start, y postion, x length
		void vert(size_t x, size_t y0, size_t y1); //x postion, y start, y length
		void drawRect(size_t x0, size_t y0, size_t x1, size_t y1); //x start, y start, x length, y length
		void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1); // start cord, end cord
	/* Text */
		void printText6x8(size_t xW, size_t yW, const char* text);
		// ResX is used if width is not set
		void printTextWarp6x8(size_t xW, size_t yW, const char* text, size_t width);
		void text6x8(size_t xW, size_t yW, char lexicon);
		// void debugPrintBuffer();

	private:
		void internal_plot(size_t x, size_t y);
		void internal_plotFast(size_t z);
		void internal_horiz(size_t x0, size_t y, size_t x1);
		void drawLine0(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
		void drawLine1(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
		uint32_t* buf; // Current buffer
		uint32_t* buf0;
		uint32_t* buf1;
		size_t ResX;
		size_t ResY;
		// size_t ResZ; // Total size
		uint32_t gColor;
		bool initialized;
};

#endif /* BITGRAPHICS_H */
