/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE.h"
#include "PortCE_Common.h"
#include "PortCE_Render.h"

#include "PortCE_include/ce/include/sys/lcd.h"
#include "PortCE_include/graphy/graphy.h"
#include "PortCE_include/keypadc/keypadc.h"
#include "PortCE_include/ce/include/ti/getcsc.h"
#include "PortCE_config/PortCE_Keybinds.h"
#include <stdio.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <pthread.h>
#include <sched.h>

static bool PortCE_SDL2_initialized = false;
static bool PortCE_invert_colors = false;
static bool PortCE_column_major = false;
static int_enum PortCE_scale_mode = SDL_ScaleModeNearest;

void SPI_UNINVERT_COLORS(void) {
	PortCE_invert_colors = false;
}
void SPI_INVERT_COLORS(void) {
	PortCE_invert_colors = true;
}
void SPI_ROW_MAJOR(void) {
	PortCE_column_major = false;
}
void SPI_COLUMN_MAJOR(void) {
	PortCE_column_major = true;
}

static int32_t RESX_MINIMUM = LCD_RESX;
static int32_t RESY_MINIMUM = LCD_RESY;
static int32_t RESX_MAXIMUM = LCD_RESX * 32;
static int32_t RESY_MAXIMUM = LCD_RESY * 32;

/* 16bit to 24bit lookup tables */
static uint8_t PreCalc_RGB1555[65536 * 3];
static uint8_t PreCalc_BGR1555[65536 * 3];
static uint8_t PreCalc_RGB565[65536 * 3];
static uint8_t PreCalc_BGR565[65536 * 3];
static uint8_t PreCalc_RGB555[65536 * 3];
static uint8_t PreCalc_BGR555[65536 * 3];
static uint8_t PreCalc_RGB444[65536 * 3];
static uint8_t PreCalc_BGR444[65536 * 3];

static void Calculate16BitColor(void) {
	{ // 1555
		size_t z = 0;
		for (uint32_t i = 0; i < 65536; i++) {
			uint16_t c = (uint16_t)i;
			uint8_t r = (uint8_t)(c & 0x1F);
			uint8_t g = (uint8_t)((c & 0x3E0) >> 4) + ((c & 0x8000) ? 1 : 0);
			uint8_t b = (uint8_t)((c & 0x7C00) >> 10);
			r *= 8; g *= 4; b *= 8;
			r += r / 32;
			g += g / 64;
			b += b / 32;
			PreCalc_RGB1555[z] = r; PreCalc_BGR1555[z] = b; z++;
			PreCalc_RGB1555[z] = g; PreCalc_BGR1555[z] = g; z++;
			PreCalc_RGB1555[z] = b; PreCalc_BGR1555[z] = r; z++;
		}
	}
	{ // 565
		size_t z = 0;
		for (uint32_t i = 0; i < 65536; i++) {
			uint16_t c = (uint16_t)i;
			uint8_t r = (uint8_t)(c & 0x1F);
			uint8_t g = (uint8_t)((c & 0x7E0) >> 4);
			uint8_t b = (uint8_t)((c & 0xF800) >> 11);
			r *= 8; g *= 4; b *= 8;
			r += r / 32;
			g += g / 64;
			b += b / 32;
			PreCalc_RGB565[z] = r; PreCalc_BGR565[z] = b; z++;
			PreCalc_RGB565[z] = g; PreCalc_BGR565[z] = g; z++;
			PreCalc_RGB565[z] = b; PreCalc_BGR565[z] = r; z++;
		}
	}
	{ // 555
		size_t z = 0;
		for (uint32_t i = 0; i < 65536; i++) {
			uint16_t c = (uint16_t)i;
			uint8_t r = (uint8_t)(c & 0x1F);
			uint8_t g = (uint8_t)((c & 0x3E0) >> 5);
			uint8_t b = (uint8_t)((c & 0x7C00) >> 10);
			r *= 8; g *= 8; b *= 8;
			r += r / 32;
			g += g / 32;
			b += b / 32;
			PreCalc_RGB555[z] = r; PreCalc_BGR555[z] = b; z++;
			PreCalc_RGB555[z] = g; PreCalc_BGR555[z] = g; z++;
			PreCalc_RGB555[z] = b; PreCalc_BGR555[z] = r; z++;
		}
	}
	{ // 444
		size_t z = 0;
		for (uint32_t i = 0; i < 65536; i++) {
			uint16_t c = (uint16_t)i;
			uint8_t r = (uint8_t)(c & 0xF);
			uint8_t g = (uint8_t)((c & 0xF0) >> 4);
			uint8_t b = (uint8_t)((c & 0xF00) >> 8);
			r *= 16; g *= 16; b *= 16;
			r += r / 16;
			g += g / 16;
			b += b / 16;
			PreCalc_RGB444[z] = r; PreCalc_BGR444[z] = b; z++;
			PreCalc_RGB444[z] = g; PreCalc_BGR444[z] = g; z++;
			PreCalc_RGB444[z] = b; PreCalc_BGR444[z] = r; z++;
		}
	}
}

/* Modern Code */

struct BufferBox {
	uint8_t* vram;
	int32_t resX;
	int32_t resY;
	size_t pitch;
}; typedef struct BufferBox BufferBox;

static BufferBox Master;

static SDL_Event event;
static SDL_Window* window;
static SDL_Texture* texture;
static SDL_Renderer *renderer;

uint32_t PortCE_get_mouse_state(int32_t* posX, int32_t* posY) {
	if (PortCE_SDL2_initialized == false) {
		return 0;
	}
	int32_t x, y;
	uint32_t state = SDL_GetMouseState(&x,&y);
	int32_t window_ResX, window_ResY;
	SDL_GetWindowSize(window, &window_ResX, &window_ResY);

	int32_t offsetX = 0;
	int32_t offsetY = 0;
	int32_t image_ResX = window_ResX;
	int32_t image_ResY = window_ResY;
	{
		const fp64 LCD_AspectRatio = (fp64)LCD_RESX / (fp64)LCD_RESY;
		const fp64 LCD_AspectRatio_Inverse = (fp64)LCD_RESY / (fp64)LCD_RESX;
		const fp64 window_AspectRatio = (fp64)window_ResX / (fp64)window_ResY;

		if (window_AspectRatio > LCD_AspectRatio) {
			image_ResX = (int32_t)((fp64)window_ResY * LCD_AspectRatio);
			offsetX = (window_ResX - image_ResX) / 2;
		} else if (window_AspectRatio < LCD_AspectRatio) {
			image_ResY = (int32_t)((fp64)window_ResX * LCD_AspectRatio_Inverse);
			offsetY = (window_ResY - image_ResY) / 2;
		}
	}

	x = ((x - offsetX) * LCD_RESX) / (image_ResX);
	y = ((y - offsetY) * LCD_RESY) / (image_ResY);

	if (posX != NULL) { *posX = x; }
	if (posY != NULL) { *posY = y; }
	return state;
}

/* Pointers */

SDL_Event* grab_SDL2_event() {
	return &event;
}

const int32_t pitch = LCD_RESX * 3;

uint8_t videoCopy[153600];
uint16_t paletteRAM[256];
uint8_t colorR[256];
uint8_t colorG[256];
uint8_t colorB[256];

struct bound {
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
};
struct bound expandedPos[56] = {
    {1,1,4,2},{6,1,4,2},{11,1,4,2},{16,1,4,2},{21,1,4,2}, //F1-F5
    {19,4,3,3},{19,10,3,3},{16,7,3,3},{22,7,3,3},//Orthagonal UDLR
    {22,4,3,3},{16,10,3,3},{16,4,3,3},{22,10,3,3},//Diagonal Shift Clockwise
    {1,5,4,3},{6,5,4,3},{11,5,4,3}, //2nd
    {1,9,4,3},{6,9,4,3},{11,9,4,3}, //Alpha
    {1,14,4,2},{6,14,4,2},{11,14,4,2},{16,14,4,2},{21,14,4,2},
    {1,17,4,2},{6,17,4,2},{11,17,4,2},{16,17,4,2},{21,17,4,2},
    {1,20,4,2},{6,20,4,2},{11,20,4,2},{16,20,4,2},{21,20,4,2},

    {1,23,4,2},{6,24,4,2},{11,24,4,2},{16,24,4,2},{21,23,4,2},
    {1,26,4,2},{6,27,4,2},{11,27,4,2},{16,27,4,2},{21,26,4,2},
    {1,29,4,2},{6,30,4,2},{11,30,4,2},{16,30,4,2},{21,29,4,2},
    {1,32,4,2},{6,33,4,2},{11,33,4,2},{16,33,4,2},{21,32,4,2},
    {1,35,4,2},{1,36,4,1} //On
};

/* Replacement Libary Functions */

const uint8_t* KEYS;
int KEYCOUNT;

// enum keyBit {
//     kb_Graph = 1    , kb_Trace = 2  , kb_Zoom = 4   , kb_Window = 8 , kb_Yequ = 16  , kb_2nd = 32   , kb_Mode = 64  , kb_Del = 128      ,
//                       kb_Sto = 2    , kb_Ln = 4     , kb_Log = 8    , kb_Square = 16, kb_Recip = 32 , kb_Math = 64  , kb_Alpha = 128    ,
//     kb_0 = 1        , kb_1 = 2      , kb_4 = 4      , kb_7 = 8      , kb_Comma = 16 , kb_Sin = 32   , kb_Apps = 64  , kb_GraphVar = 128 ,
//     kb_DecPnt = 1   , kb_2 = 2      , kb_5 = 4      , kb_8 = 8      , kb_LParen = 16, kb_Cos = 32   , kb_Prgm = 64  , kb_Stat = 128     ,
//     kb_Chs = 1      , kb_3 = 2      , kb_6 = 4      , kb_9 = 8      , kb_RParen = 16, kb_Tan = 32   , kb_Vars = 64  ,
//     kb_Enter = 1    , kb_Add = 2    , kb_Sub = 4    , kb_Mul = 8    , kb_Div = 16   , kb_Power = 32 , kb_Clear = 64 ,
//     kb_Down = 1     , kb_Left = 2   , kb_Right = 4  , kb_Up = 8
// };

#define internal_kb_Data(k) (((volatile uint16_t*)RAM_ADDRESS(0xF50010))[k])

static void setKey(uint8_t k) {
    if (k >= 64) { //Out of Bounds
        return;
    }
    uint8_t bit = 1 << (k % 8);
    uint8_t off = k / 8;
    internal_kb_Data(off + 1) |= (bit); //+1 otherwise down becomes enter
}
static void resetKey(uint8_t k) {
    if (k >= 64) { //Out of Bounds
        return;
    }
    uint8_t bit = 1 << (k % 8);
    uint8_t off = k / 8;
    internal_kb_Data(off + 1) &= ~(bit);
}

/* Replacement Libary Functions */

 //Allows for the same key to be binded to more than one thing

static uint8_t internal_kb_Scan(void) {
	static uint8_t tempKey[56];
    SDL_PollEvent(&event);
    KEYS = SDL_GetKeyboardState(&KEYCOUNT);
    uint32_t length = sizeof(PortCE_Keybind_Selection)/sizeof(PortCE_Keybind_Selection[0]);
    for (uint32_t i = 0; i < length; i++) {
        if (KEYS[PortCE_Keybind_Selection[i].SDL] == 1) {
            //printf("%s\n",SDL_GetKeyName(PortCE_Keybind_Selection[i].SDL));
            //printf("%d %d,",q,PortCE_Keybind_Selection[i].CE); fflush(stdout);
            setKey(PortCE_Keybind_Selection[i].CE);
            tempKey[PortCE_Keybind_Selection[i].CE] = 1;
        }
    }
	uint8_t pressed_key_count = 0;
    for (uint8_t i = 0; i < 56; i++) {
        if (tempKey[i] == 0) {
            resetKey(i);
        }
		if (tempKey[i] == 1) {
			pressed_key_count++;
		}
        tempKey[i] = 0;
    }
	return pressed_key_count;
}

void kb_Scan(void) {
	internal_kb_Scan();
}


kb_key_t kb_ScanGroup(uint8_t row) {
	internal_kb_Scan();
	return internal_kb_Data(row);
}


uint8_t kb_AnyKey(void) {
	return internal_kb_Scan();
}


void kb_Reset(void) {
    for (uint8_t i = 0; i < 7; i++) {
        internal_kb_Data(i) = 0;
    }
}

bool boot_CheckOnPressed(void) {
	return false; // No way to check for the ON key currently
}

static uint8_t internal_CSC_Scan(void) {
    SDL_PollEvent(&event);
    KEYS = SDL_GetKeyboardState(&KEYCOUNT);
    uint32_t length = sizeof(PortCE_Keybind_Selection)/sizeof(PortCE_Keybind_Selection[0]);
    for (uint32_t i = 0; i < length; i++) {
        if (KEYS[PortCE_Keybind_Selection[i].SDL] == 1) {
            return PortCE_Keybind_Selection[i].CE;
        }
    }
	return 0;
}

uint8_t os_GetCSC(void) {
	uint8_t key = internal_CSC_Scan();
	switch(key) {
		case KB_Down    : return sk_Down    ;
		case KB_Left    : return sk_Left    ;
		case KB_Right   : return sk_Right   ;
		case KB_Up      : return sk_Up      ;
		case KB_Enter   : return sk_Enter   ;
		case KB_2nd     : return sk_2nd     ;
		case KB_Clear   : return sk_Clear   ;
		case KB_Alpha   : return sk_Alpha   ;
		case KB_Add     : return sk_Add     ;
		case KB_Sub     : return sk_Sub     ;
		case KB_Mul     : return sk_Mul     ;
		case KB_Div     : return sk_Div     ;
		case KB_Graph   : return sk_Graph   ;
		case KB_Trace   : return sk_Trace   ;
		case KB_Zoom    : return sk_Zoom    ;
		case KB_Window  : return sk_Window  ;
		case KB_Yequ    : return sk_Yequ    ;
		case KB_Mode    : return sk_Mode    ;
		case KB_Del     : return sk_Del     ;
		case KB_Sto     : return sk_Store   ;
		case KB_Ln      : return sk_Ln      ;
		case KB_Log     : return sk_Log     ;
		case KB_Square  : return sk_Square  ;
		case KB_Recip   : return sk_Recip   ;
		case KB_Math    : return sk_Math    ;
		case KB_0       : return sk_0       ;
		case KB_1       : return sk_1       ;
		case KB_4       : return sk_4       ;
		case KB_7       : return sk_7       ;
		case KB_2       : return sk_2       ;
		case KB_5       : return sk_5       ;
		case KB_8       : return sk_8       ;
		case KB_3       : return sk_3       ;
		case KB_6       : return sk_6       ;
		case KB_9       : return sk_9       ;
		case KB_Comma   : return sk_Comma   ;
		case KB_Sin     : return sk_Sin     ;
		case KB_Apps    : return sk_Apps    ;
		case KB_GraphVar: return sk_GraphVar;
		case KB_DecPnt  : return sk_DecPnt  ;
		case KB_LParen  : return sk_LParen  ;
		case KB_Cos     : return sk_Cos     ;
		case KB_Prgm    : return sk_Prgm    ;
		case KB_Stat    : return sk_Stat    ;
		case KB_Chs     : return sk_Chs     ;
		case KB_RParen  : return sk_RParen  ;
		case KB_Tan     : return sk_Tan     ;
		case KB_Vars    : return sk_Vars    ;
		case KB_Power   : return sk_Power   ;
		default: return 0;
	}
}

static void blit16bpp(uint8_t* dst_buf, const uint8_t* src_buf) {
	uint8_t* PreCalc16;
	uint16_t colorMode = (lcd_VideoMode & LCD_MASK_BBP);
	switch (colorMode) {
		case LCD_MASK_COLOR1555:
			PreCalc16 = (lcd_VideoMode & LCD_MASK_BGR) ? PreCalc_BGR1555 : PreCalc_RGB1555;
		break;
		case LCD_MASK_COLOR565:
			PreCalc16 = (lcd_VideoMode & LCD_MASK_BGR) ? PreCalc_BGR565 : PreCalc_RGB565;
		break;
		case LCD_MASK_COLOR444:
			PreCalc16 = (lcd_VideoMode & LCD_MASK_BGR) ? PreCalc_BGR444 : PreCalc_RGB444;
		break;
		default:
			PreCalc16 = (lcd_VideoMode & LCD_MASK_BGR) ? PreCalc_BGR555 : PreCalc_RGB555;
	};
	size_t w = 0;
	size_t z = 0;
	for (uint32_t y = 0; y < LCD_RESY; y++) {
		for (uint32_t x = 0; x < LCD_RESX; x++) {
			uint32_t c = (uint32_t)((const uint16_t*)src_buf)[z];
			c *= 3;
			dst_buf[w] = PreCalc16[c]; w++;
			dst_buf[w] = PreCalc16[c + 1]; w++;
			dst_buf[w] = PreCalc16[c + 2]; w++;
			z += PortCE_column_major ? LCD_RESY : 1;
		}
		z -= PortCE_column_major ? ((LCD_RESX * LCD_RESY) - 1) : 0;
		//w += pitch - (LCD_RESX * 3);
	}
}

static void blit8bpp(uint8_t* dst_buf, const uint8_t* src_buf) {
	size_t w = 0;
	size_t z = 0;
	for (uint32_t y = 0; y < (uint32_t)LCD_RESY; y++) {
		for (uint32_t x = 0; x < (uint32_t)LCD_RESX; x++) {
			uint8_t c = src_buf[z];
			dst_buf[w] = colorR[c]; w++;
			dst_buf[w] = colorG[c]; w++;
			dst_buf[w] = colorB[c]; w++;
			z += PortCE_column_major ? LCD_RESY : 1;
		}
		z -= PortCE_column_major ? ((LCD_RESX * LCD_RESY) - 1) : 0;
		//w += pitch - (LCD_RESX * lenY * 3);
	}
}

static void blit4bpp(uint8_t* dst_buf, const uint8_t* src_buf) {
	const uint32_t PixelsPerByte = 2;
	size_t w = 0;
	size_t z = 0;
	if (PortCE_column_major == false) {
		for (uint32_t y = 0; y < LCD_RESY; y++) {
			for (uint32_t x = 0; x < LCD_RESX / PixelsPerByte; x++) {
				uint8_t c = src_buf[z];
				dst_buf[w] = colorR[c & 0xF]; w++;
				dst_buf[w] = colorG[c & 0xF]; w++;
				dst_buf[w] = colorB[c & 0xF]; w++;
				dst_buf[w] = colorR[(c >> 4) & 0xF]; w++;
				dst_buf[w] = colorG[(c >> 4) & 0xF]; w++;
				dst_buf[w] = colorB[(c >> 4) & 0xF]; w++;
				z++;
			}
			//w += pitch - (LCD_RESX * lenY * 3);
		}
	} else {
		for (uint32_t x = 0; x < LCD_RESX; x++) {
			for (uint32_t y = 0; y < LCD_RESY; y += PixelsPerByte) {
				uint8_t c = src_buf[z];
				for (uint32_t i = 0; i < PixelsPerByte; i++) {
					dst_buf[w] = colorR[c & 0xF]; w++;
					dst_buf[w] = colorG[c & 0xF]; w++;
					dst_buf[w] = colorB[c & 0xF]; w++;
					w += (pitch  - 3);
					c >>= 4;
				}
				z++;
			}
			w -= pitch * LCD_RESY;
			w += 3;
		}
	}
}

static void blit2bpp(uint8_t* dst_buf, const uint8_t* src_buf) {
	const uint32_t PixelsPerByte = 4;
	size_t w = 0;
	size_t z = 0;
	if (PortCE_column_major == false) {
		for (uint32_t y = 0; y < LCD_RESY; y++) {
			for (uint32_t x = 0; x < LCD_RESX / PixelsPerByte; x++) {
				uint8_t c = src_buf[z];
				dst_buf[w] = colorR[c & 0x3]; w++;
				dst_buf[w] = colorG[c & 0x3]; w++;
				dst_buf[w] = colorB[c & 0x3]; w++;
				dst_buf[w] = colorR[(c >> 2) & 0x3]; w++;
				dst_buf[w] = colorG[(c >> 2) & 0x3]; w++;
				dst_buf[w] = colorB[(c >> 2) & 0x3]; w++;
				dst_buf[w] = colorR[(c >> 4) & 0x3]; w++;
				dst_buf[w] = colorG[(c >> 4) & 0x3]; w++;
				dst_buf[w] = colorB[(c >> 4) & 0x3]; w++;
				dst_buf[w] = colorR[(c >> 6) & 0x3]; w++;
				dst_buf[w] = colorG[(c >> 6) & 0x3]; w++;
				dst_buf[w] = colorB[(c >> 6) & 0x3]; w++;
				z++;
			}
			//w += pitch - (LCD_RESX * 3);
		}
	} else {
		for (uint32_t x = 0; x < LCD_RESX; x++) {
			for (uint32_t y = 0; y < LCD_RESY; y += PixelsPerByte) {
				uint8_t c = src_buf[z];
				for (uint32_t i = 0; i < PixelsPerByte; i++) {
						dst_buf[w] = colorR[c & 0x3]; w++;
						dst_buf[w] = colorG[c & 0x3]; w++;
						dst_buf[w] = colorB[c & 0x3]; w++;
					w += (pitch - 3);
					c >>= 2;
				}
				z++;
			}
			w -= pitch * LCD_RESY;
			w += 3;
		}
	}
}

static void blit1bpp(uint8_t* dst_buf, const uint8_t* src_buf) {
	const uint32_t PixelsPerByte = 8;
	size_t w = 0;
	size_t z = 0;
	if (PortCE_column_major == false) {
		for (uint32_t y = 0; y < LCD_RESY; y++) {
			for (uint32_t x = 0; x < LCD_RESX / PixelsPerByte; x++) {
				uint8_t c = src_buf[z];
				for (uint8_t b = 0; b < 8; b++) {
					dst_buf[w] = colorR[(c >> b) & 0x1]; w++;
					dst_buf[w] = colorG[(c >> b) & 0x1]; w++;
					dst_buf[w] = colorB[(c >> b) & 0x1]; w++;
				}
				z++;
			}
			//w += pitch - (LCD_RESX * 3);
		}
	} else {
		for (uint32_t x = 0; x < LCD_RESX; x++) {
			for (uint32_t y = 0; y < LCD_RESY; y += PixelsPerByte) {
				uint8_t c = src_buf[z];
				for (uint32_t i = 0; i < PixelsPerByte; i++) {
						dst_buf[w] = colorR[c & 0x1]; w++;
						dst_buf[w] = colorG[c & 0x1]; w++;
						dst_buf[w] = colorB[c & 0x1]; w++;
					w += (pitch - 3);
					c >>= 1;
				}
				z++;
			}
			w -= pitch * LCD_RESY;
			w += 3;
		}
	}
}

// Will be optimized later on
static void renderCursor(uint8_t* data) {
	if (data == NULL) { return; }
	// srand(1234);
	// for (size_t i = 0; i < 1024; i++) {
	// 	lcd_CrsrImage[i] = rand() % 256;
	// }
	const bool cursorEnabled = (lcd_CrsrCtrl & 0x1) ? true : false;
	// printf("\nCursor: %s", cursorEnabled ? "true" : "false");
	// printf("\n\tPos: %hu,%hhu Clip: %hhu,%hhu %u", lcd_CrsrX, lcd_CrsrY, lcd_CrsrClipX, lcd_CrsrClipY, lcd_CrsrClip);
	if (cursorEnabled == false) { return; }
	const bool fullCursor = (lcd_CrsrConfig & 0x1) ? true : false;

	const uint8_t pixelsPerByte = 4;
	const uint8_t bitsPerPixel = 2;

	const uint8_t cursorDim = fullCursor ? 64 : 32;

	bool use_columnMajorCursor = PortCE_column_major && false;
	const uint16_t cursor_PosX  = use_columnMajorCursor ? lcd_CrsrY     : lcd_CrsrX    ;
	const uint8_t  cursor_PosY  = use_columnMajorCursor ? lcd_CrsrX     : lcd_CrsrY    ;
	const uint8_t  cursor_ClipX = use_columnMajorCursor ? lcd_CrsrClipY : lcd_CrsrClipX;
	const uint8_t  cursor_ClipY = use_columnMajorCursor ? lcd_CrsrClipX : lcd_CrsrClipY;

	if (cursor_ClipX >= cursorDim || cursor_ClipY >= cursorDim) {
		// printf("\nError: Everything is clipped");
		return; // Everything is clipped
	}
	const int32_t LCD_RESV = PortCE_column_major ? LCD_RESX : LCD_RESY;
	if (cursor_PosX >= LCD_RESV || cursor_PosY >= LCD_RESY) {
		// printf("\nError: Out of bounds");
		return; // Out of bounds
	}
	const uint16_t limitX = (cursor_PosX + cursorDim > LCD_RESV) ? ( (uint16_t)cursorDim - ((          cursor_PosX + (uint16_t)cursorDim) - LCD_RESV) ) : cursorDim;
	const uint16_t limitY = (cursor_PosY + cursorDim > LCD_RESY) ? ( (uint16_t)cursorDim - (((uint16_t)cursor_PosY + (uint16_t)cursorDim) - LCD_RESY) ) : cursorDim;
	if (fullCursor) {
		for (uint16_t y = cursor_ClipY; y < limitY; y++) {
			for (uint16_t x = cursor_ClipX; x < limitX; x++) {
				const size_t data_Index = 3 * ( ((size_t)((uint16_t)cursor_PosY + (uint16_t)y) * LCD_RESX) + (size_t)((uint16_t)cursor_PosX + (uint16_t)x) );
				const size_t cursor_Index = ((size_t)y * (size_t)cursorDim) + (size_t)x;
				const uint8_t pixel = (lcd_CrsrImage[cursor_Index / pixelsPerByte] >> ((3 - (cursor_Index % pixelsPerByte)) * bitsPerPixel)) & 0b11;
				switch (pixel) {
					case lcd_CrsrPixelPalette0:
						data[data_Index + 0] = lcd_CrsrPalette0 & 0xFF;
						data[data_Index + 1] = (lcd_CrsrPalette0 >> 8) & 0xFF;
						data[data_Index + 2] = (lcd_CrsrPalette0 >> 16) & 0xFF;
						break;
					case lcd_CrsrPixelPalette1:
						data[data_Index + 0] = lcd_CrsrPalette1 & 0xFF;
						data[data_Index + 1] = (lcd_CrsrPalette1 >> 8) & 0xFF;
						data[data_Index + 2] = (lcd_CrsrPalette1 >> 16) & 0xFF;
						break;
					case lcd_CrsrPixelTransparent:
						/* Transparent */
						break;
					case lcd_CrsrPixelInvert:
						/* Invert Colors */
						data[data_Index + 0] = ~data[data_Index + 0];
						data[data_Index + 1] = ~data[data_Index + 1];
						data[data_Index + 2] = ~data[data_Index + 2];
						break;
				}
			}
		}
	} else {
		const size_t cursor_Offset = (size_t)((lcd_CrsrCtrl >> 4) & 0b11) * 0x100;
		for (uint16_t y = lcd_CrsrClipY; y < limitY; y++) {
			for (uint16_t x = lcd_CrsrClipX; x < limitX; x++) {
				const size_t data_Index = 3 * ( ((size_t)((uint16_t)lcd_CrsrY + (uint16_t)y) * LCD_RESX) + (size_t)((uint16_t)lcd_CrsrX + (uint16_t)x) );
				const size_t cursor_Index = ((size_t)y * (size_t)cursorDim) + (size_t)x + cursor_Offset;
				const uint8_t pixel = (lcd_CrsrImage[cursor_Index / pixelsPerByte] >> ((cursor_Index % pixelsPerByte) * bitsPerPixel)) & 0b11;
				switch (pixel) {
					case lcd_CrsrPixelPalette0:
						data[data_Index + 0] = lcd_CrsrPalette0 & 0xFF;
						data[data_Index + 1] = (lcd_CrsrPalette0 >> 8) & 0xFF;
						data[data_Index + 2] = (lcd_CrsrPalette0 >> 16) & 0xFF;
						break;
					case lcd_CrsrPixelPalette1:
						data[data_Index + 0] = lcd_CrsrPalette1 & 0xFF;
						data[data_Index + 1] = (lcd_CrsrPalette1 >> 8) & 0xFF;
						data[data_Index + 2] = (lcd_CrsrPalette1 >> 16) & 0xFF;
						break;
					case lcd_CrsrPixelTransparent:
						/* Transparent */
						break;
					case lcd_CrsrPixelInvert:
						/* Invert Colors */
						data[data_Index + 0] = ~data[data_Index + 0];
						data[data_Index + 1] = ~data[data_Index + 1];
						data[data_Index + 2] = ~data[data_Index + 2];
						break;
				}
			}
		}
	}
}

#ifdef Debug_Print_LCD_Registers
	void internal_print_LCD_registers(void) {
		#define PrintVar(label, value) \
			printf("\t%12s: %" PRId32 "\n", (label), (int32_t)(value));
		
		#define PrintHex(label, value) \
			printf("\t%12s: 0x%06" PRIX32 "\n", (label), (int32_t)(value));

		printf("LCD_registers:\n");
		PrintHex("lcd_Control", lcd_Control);
		PrintHex("lcd_UpBase", lcd_UpBase);
		PrintHex("lcd_LpBase", lcd_LpBase);
		#undef PrintHex
		#undef PrintVar
	}
#endif

/**
 * @brief Writes the contents of the ti84ce screen to a buffer
 * 
 * @param data buffer to write a LCD_RESX * LCD_RESY image to
 */
void copyFrame(uint8_t* data) {
	#ifdef Debug_Print_LCD_Registers
		internal_print_LCD_registers();
	#endif
	memcpy(paletteRAM,lcd_Palette,256 * sizeof(uint16_t));
	size_t copyAmount = 0;
	uint16_t colorMode = (lcd_VideoMode & LCD_MASK_BBP);
	switch (colorMode) {
		case LCD_MASK_INDEXED1:
			copyAmount = (LCD_RESX * LCD_RESY) / 8;
		break;
		case LCD_MASK_INDEXED2:
			copyAmount = (LCD_RESX * LCD_RESY) / 4;
		break;
		case LCD_MASK_INDEXED4:
			copyAmount = (LCD_RESX * LCD_RESY) / 2;
		break;
		case LCD_MASK_INDEXED8:
			copyAmount = (LCD_RESX * LCD_RESY);
		break;
		case LCD_MASK_COLOR1555:
			copyAmount = (LCD_RESX * LCD_RESY) * 2;
		break;
		case LCD_MASK_COLOR565:
			copyAmount = (LCD_RESX * LCD_RESY) * 2;
		break;
		case LCD_MASK_COLOR444:
			copyAmount = (LCD_RESX * LCD_RESY) * 2;
		break;
		default:
			copyAmount = (LCD_RESX * LCD_RESY) * 2;
	};

	memcpy(videoCopy,((uint8_t*)&simulated_ram[(0xD00000 | (lcd_UpBase & (0xFFFF << 3)))]),copyAmount);
	

	// Tests BGR bit
    if (lcd_VideoMode & LCD_MASK_BGR) {
        //Converts 1555 to 888 Color
        for (uint32_t i = 0; i < 256; i++) {
            uint16_t col = paletteRAM[i];
            colorB[i] = (col & 31) << 3;
            col >>= 5;
            colorG[i] = col & 1024 ? 4 : 0;
            colorG[i] += (col & 31) << 3;
            col >>= 5;
            colorR[i] = (col & 31) << 3;
        }
        //Scales color (Otherwise white is 248,252,248)
        for (uint32_t i = 0; i < 256; i++) { //Magic Numbers: 35,84 Floor 32,64 Round
            colorB[i] += (colorB[i] / 32);
            colorG[i] += (colorG[i] / 64);
            colorR[i] += (colorR[i] / 32);
        }
    } else {
        //Converts 1555 to 888 Color
        for (uint32_t i = 0; i < 256; i++) {
            uint16_t col = paletteRAM[i];
            colorR[i] = (col & 31) << 3;
            col >>= 5;
            colorG[i] = col & 1024 ? 4 : 0;
            colorG[i] += (col & 31) << 3;
            col >>= 5;
            colorB[i] = (col & 31) << 3;
        }
        //Scales color (Otherwise white is 248,252,248)
        for (uint32_t i = 0; i < 256; i++) { //Magic Numbers: 35,84 Floor 32,64 Round
            colorR[i] += (colorB[i] / 32);
            colorG[i] += (colorG[i] / 64);
            colorB[i] += (colorR[i] / 32);
        }
    }
    if (PortCE_invert_colors) {
        for (uint32_t i = 0; i < 256; i++) {
            colorB[i] = ~colorB[i];
            colorG[i] = ~colorG[i];
            colorR[i] = ~colorR[i];
        }
    }
	switch (colorMode) {
		case LCD_MASK_INDEXED1:
			blit1bpp(data, videoCopy);
		break;
		case LCD_MASK_INDEXED2:
			blit2bpp(data, videoCopy);
		break;
		case LCD_MASK_INDEXED4:
			blit4bpp(data, videoCopy);
		break;
		case LCD_MASK_INDEXED8:
			blit8bpp(data, videoCopy);
		break;
		case LCD_MASK_COLOR1555:
			blit16bpp(data, videoCopy);
		break;
		case LCD_MASK_COLOR565:
			blit16bpp(data, videoCopy);
		break;
		case LCD_MASK_COLOR444:
			blit16bpp(data, videoCopy);
		break;
		default:
			blit16bpp(data, videoCopy);
	};
	renderCursor(data);
}

/*
**	===========
**	Modern Code
**	===========
*/

void PortCE_initialize_sound(void);
void PortCE_terminate_sound(void);

int terminateLCDcontroller() {
	PortCE_terminate_sound();
	PortCE_SDL2_initialized = false;
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	FREE(Master.vram);
    return EXIT_SUCCESS;
}

void initLCDcontroller(const char* window_title, const PortCE_Config* config) {
	
	uint8_t init_scale = (config == NULL) ? 2 : config->window_scale;
	if (init_scale < 1) {
		init_scale = 1;
	} else if(init_scale > 8) {
		init_scale = 8;
	}

	PortCE_scale_mode = config->linear_interpolation ? SDL_ScaleModeLinear : SDL_ScaleModeNearest;

	Master.resX = LCD_RESX;
	Master.resY = LCD_RESY;
	Master.vram = NULL;
	Master.pitch = Master.resX * 3;
	Master.vram = calloc((size_t)Master.resY * Master.pitch, sizeof(uint8_t));
	if (Master.vram == NULL) { printf("\nFailed to calloc Master.vram"); fflush(stdout); return; }
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
		window_title,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		LCD_RESX * init_scale, LCD_RESY * init_scale,
		SDL_WINDOW_RESIZABLE
	);

	SDL_SetWindowMinimumSize(window, RESX_MINIMUM, RESY_MINIMUM);
	SDL_SetWindowMaximumSize(window, RESX_MAXIMUM, RESY_MAXIMUM);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
		LCD_RESX * init_scale, LCD_RESY * init_scale
	);
	SDL_SetTextureScaleMode(texture, PortCE_scale_mode);

	PortCE_initialize_sound();

	Calculate16BitColor();
	PortCE_SDL2_initialized = true;
}

// Returns True if the window was resized. Optional: Returns new window size.
static bool resizeWindow(int32_t resX, int32_t resY, uint32_t* resizeX, uint32_t* resizeY) {
	bool reVal = false;
	static int32_t rX = 0, rY = 0;
	if (rX != resX || rY != resY) {
		valueLimit(resX,RESX_MINIMUM,RESX_MAXIMUM);
		valueLimit(resY,RESY_MINIMUM,RESY_MAXIMUM);

		// Rounds resX down to the nearest multiple of 4 to eliminate padding
		if (resX & 0x3) { resX = resX & ~0x3; }

		if (Master.vram == NULL) {
			if (texture == NULL) {
				printf("\nError: realloc Master.vram failed while resizing window");
				FREE(Master.vram);
				terminateLCDcontroller();
				return true;
			}
		}

		SDL_SetWindowSize(window,resX,resY);

		SDL_RenderSetLogicalSize(renderer, resX, resY);
		
		if (resizeX != NULL) { *resizeX = resX; }
		if (resizeY != NULL) { *resizeY = resY; }

		if (texture != NULL) {
			SDL_DestroyTexture(texture);
		}
		
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);
		SDL_SetTextureScaleMode(texture, PortCE_scale_mode);
		if (texture == NULL) {
			printf("\nError: SDL_CreateTexture failed while resizing window");
			terminateLCDcontroller();
			return true;
		}

		memset(Master.vram,0,Master.pitch * Master.resY);
		reVal = true;
	}
	rX = resX;
	rY = resY;
	return reVal;
}

// Returns True if the window was resized. Optional: Returns new window size.
static bool windowResizingCode(uint32_t* resX, uint32_t* resY) {
	int32_t x = 0, y = 0;
	SDL_GetWindowSize(window, &x, &y);
	return resizeWindow(x, y, resX, resY);
}

static void pace_frame(nano64_t pace_time) {
	static nano64_t last_frame_time = 0;

	const nano64_t yield_threshold = SECONDS_TO_NANO(1.0e-4);

	nano64_t current_time = getNanoTime();
	while (current_time - pace_time + yield_threshold < last_frame_time) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			terminateLCDcontroller();
			exit(0);
		}
		sched_yield();
		current_time = getNanoTime();
	}
	while (current_time - pace_time < last_frame_time) {
		current_time = getNanoTime();
	}
	last_frame_time = current_time;
}

void PortCE_new_frame(void) {
	PortCE_update_registers();
	if (PortCE_SDL2_initialized == false) {
		return;
	}
	// nano64_t startTime = getNanoTime();

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
        terminateLCDcontroller();
        exit(0);
    }
	windowResizingCode(NULL,NULL);
	copyFrame(Master.vram);
	
	SDL_UpdateTexture(texture, NULL, Master.vram, Master.pitch);
	{
		// SDL_Rect srcRect = {0,0,(int)Master.resX,(int)Master.resY};
		int window_ResX, window_ResY;
		SDL_GetWindowSize(window, &window_ResX, &window_ResY);
		SDL_Rect dstRect = {0,0,window_ResX,window_ResY};

		const fp64 LCD_AspectRatio = (fp64)LCD_RESX / (fp64)LCD_RESY;
		const fp64 LCD_AspectRatio_Inverse = (fp64)LCD_RESY / (fp64)LCD_RESX;
		const fp64 window_AspectRatio = (fp64)window_ResX / (fp64)window_ResY;

		if (window_AspectRatio > LCD_AspectRatio) {
			int image_ResX = (int)((fp64)window_ResY * LCD_AspectRatio);
			dstRect = (SDL_Rect) {
				(window_ResX - image_ResX) / 2, 0,
				image_ResX, window_ResY
			};
		} else if (window_AspectRatio < LCD_AspectRatio) {
			int image_ResY = (int)((fp64)window_ResX * LCD_AspectRatio_Inverse);
			dstRect = (SDL_Rect){
				0, (window_ResY - image_ResY) / 2,
				window_ResX, image_ResY
			};
		}
		
		SDL_RenderCopy(renderer, texture, NULL, &dstRect);
	}
	SDL_RenderPresent(renderer);

	// nano64_t endTime = getNanoTime();
	// printf("\nTime: %.3lfms | %.3lffps %lld", NANO_TO_SECONDS(endTime - startTime) * 1.0e3, NANO_TO_FRAMERATE(endTime - startTime), getNanoTime());
}

void PortCE_pace_frame(float frame_rate) {
	PortCE_new_frame();
	pace_frame(FRAMERATE_TO_NANO((fp64)frame_rate));
}