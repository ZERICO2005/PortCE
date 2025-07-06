/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"
#include "./PortCE_include/ce/include/ti/ui.h"
#include "./PortCE_include/ce/include/ti/vars.h"
#include "./PortCE_include/ce/include/ti/screen.h"
#include "./PortCE_include/ce/include/sys/lcd.h"

/**
 * @todo Implement TiOS functions
 * 
 */

/* TiOS Colors */

enum TiOS_Colors {
	TiOS_BLUE      = 0x001F,
	TiOS_RED       = 0x7C00,
	TiOS_BLACK     = 0x0000,
	TiOS_MAGENTA   = 0x7C1F,
	TiOS_GREEN     = 0x8260,
	TiOS_ORANGE    = 0xFE24,
	TiOS_BROWN     = 0x5880,
	TiOS_NAVY      = 0x0010,
	TiOS_LTBLUE    = 0x025F,
	TiOS_YELLOW    = 0xFFE0,
	TiOS_WHITE     = 0xFFFF,
	TiOS_LTGRAY    = 0x6318,
	TiOS_MEDGRAY   = 0x739C,
	TiOS_GRAY      = 0x4631,
	TiOS_DARKGRAY  = 0xA94A,
	TiOS_HIGHLIGHT = 0xBFFE
};

/* Temporary Graphics */
	static const uint8_t char6x8[1536] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,223,223,6,0,7,3,0,0,7,3,228,60,39,228,60,39,36,110,202,83,118,36,66,37,18,72,164,66,118,255,153,127,230,176,0,0,7,3,0,0,0,126,195,129,129,0,0,129,129,195,126,0,17,10,4,4,10,17,48,48,252,252,48,48,0,0,160,96,0,0,16,16,16,16,16,16,0,0,192,192,0,0,224,240,24,12,7,3,126,255,145,137,255,126,0,132,254,255,128,0,198,231,177,153,207,198,98,235,137,137,255,118,48,44,35,255,255,32,111,239,137,137,251,115,126,255,137,137,251,114,3,3,225,249,31,7,118,255,137,137,255,118,78,223,145,145,255,126,0,0,102,102,0,0,0,0,86,54,0,0,8,8,20,20,34,34,20,20,20,20,20,20,34,34,20,20,8,8,6,7,209,217,15,6,126,129,181,189,161,30,240,252,39,39,252,240,255,255,137,137,255,118,126,255,129,129,230,103,255,255,129,129,255,126,255,255,137,137,235,227,255,255,9,9,11,3,126,255,129,145,118,247,255,255,8,8,255,255,0,129,255,255,129,0,96,224,128,128,255,127,255,255,24,60,231,195,255,255,128,128,224,224,255,14,28,28,14,255,255,14,28,56,112,255,126,255,129,129,255,126,255,255,17,17,31,14,30,63,97,225,191,158,255,255,49,113,223,142,102,239,137,137,251,114,3,1,255,255,1,3,127,255,128,128,255,127,15,63,224,224,63,15,255,112,56,56,112,255,227,247,28,28,247,227,3,15,248,248,15,3,227,243,153,141,231,227,0,255,129,129,129,0,3,7,12,24,240,224,0,129,129,129,255,0,4,6,3,3,6,4,128,128,128,128,128,128,0,1,3,2,0,0,96,244,148,148,124,248,255,127,132,132,252,120,120,252,132,132,204,72,120,252,132,132,127,255,120,252,148,148,220,88,16,254,255,17,19,2,24,188,164,164,248,124,255,255,8,8,248,240,0,136,251,251,128,0,128,136,251,123,0,0,255,255,16,56,236,196,0,129,255,255,128,0,252,248,4,248,4,248,252,248,4,4,252,248,120,252,132,132,252,120,252,248,36,36,60,24,24,60,36,36,248,252,252,248,4,4,28,24,72,220,148,164,236,72,0,4,126,255,132,64,124,252,128,128,124,252,28,124,192,192,124,28,60,124,128,124,128,124,196,236,56,56,236,196,140,188,224,96,60,12,204,236,164,148,220,204,8,8,126,247,129,129,0,0,255,255,0,0,129,129,247,126,8,8,16,8,24,24,16,8,63,32,32,168,112,32
	};
	static uint16_t gColor = 0;

	static void fillRect(uint24_t x1, uint24_t y1, uint24_t x2, uint24_t y2) { //x start, y start, x length, y length
		uint16_t* fill = (uint16_t*)lcd_Ram + (int24_t)(x1 + (y1 * LCD_RESX));
		for (uint24_t dY = 0; dY < y2; dY++) {
			memset(fill,gColor,x2 * sizeof(uint16_t));
			fill += LCD_RESX;
		}
	}
	__attribute__((unused)) static void horiz(uint24_t x1, uint24_t y, uint24_t x2) { //x start, y postion, x length
			uint16_t* fill = (uint16_t*)lcd_Ram + (int24_t)(x1 + (y * LCD_RESX));
			memset(fill,gColor,x2 * sizeof(uint16_t));
	}
	__attribute__((unused)) static void vert(uint24_t x, uint24_t y1, uint24_t y2) { //x postion, y start, y length
		uint16_t* z = (uint16_t*)lcd_Ram + (int24_t)(x + ((y2 + y1) * LCD_RESX));
		for (uint16_t* fill = (uint16_t*)lcd_Ram + (y1 * LCD_RESX) + x; fill < z; fill += LCD_RESX) {
			*fill = gColor;
		}
	}
	__attribute__((unused)) static void fillScreen() { 
		memset(lcd_Ram,gColor,LCD_RESX * LCD_RESY);
	}
	static void text6x8(uint24_t xW, uint24_t yW, uint8_t lexicon) { //x position, y position, letter index
		uint8_t* bitImage = (uint8_t*)char6x8 + (lexicon * 6);
		uint16_t* fill = (uint16_t*)lcd_Ram + (yW * LCD_RESX + xW);
		uint8_t b = 0x1;
		for (uint8_t y = 0; y < 8; y++) {
			for (uint8_t x = 0; x < 6; x++) {
				*fill = *bitImage & b ? gColor : *fill;
				bitImage++;
				fill++;
			}
			bitImage -= 6;
			fill += (LCD_RESX - 6);
			b <<= 1;
		}
	}
	static void str6x8(const char* str) {
		if (str == NULL) {
			return;
		}
		gColor = TiOS_BLACK;
		uint24_t posX = os_CurCol * 10;
		uint24_t posY = os_CurRow * 16;
		while (*str != '\0') {
			if (posX < LCD_RESX - 16 && posY < LCD_RESY - 10) {
				text6x8(posX, posY, *str);
				posX += 16;
				os_CurRow++;
			} else {
				return;
			}
		}
	}

	static void strPos6x8(const char* str, uint8_t curRow, uint8_t curCol) {
		if (str == NULL) {
			return;
		}
		gColor = TiOS_BLACK;
		uint24_t posX = curCol * 10;
		uint24_t posY = curRow * 16;
		while (*str != '\0') {
			if (posX < LCD_RESX - 16 && posY < LCD_RESY - 10) {
				text6x8(posX, posY, *str);
				posX += 16;
			}
		}
	}


/* <ti/ui.h> */
	void boot_ClearVRAM(void) {
		memset(lcd_Ram, 0xFF, LCD_SIZE);
	}

	void os_RunIndicOn(void) {
		gColor = TiOS_YELLOW;
		fillRect(LCD_RESX - 20,10,10,10);
	}
	void os_RunIndicOff(void) {
		gColor = TiOS_DARKGRAY;
		fillRect(LCD_RESX - 20,10,10,10);
	}
	void os_DrawStatusBar(void) {
		os_FillRectColor = TiOS_DARKGRAY;
		gColor = os_FillRectColor;
		fillRect(0,0,LCD_RESX,30);
	}
	void os_ForceCmdNoChar(void) {
		// Not sure what this function does
	}

/* <ti/screen.h> */

void boot_NewLine(void) {
	os_CurCol = 0;
	os_CurRow++;
}

void os_NewLine(void) {
	os_CurCol = 0;
	os_CurRow++;
}

void os_DisableCursor(void) {
	return;
}

void os_EnableCursor(void) {
	return;
}

void os_SetCursorPos(uint8_t curRow, uint8_t curCol) {
	os_CurRow = curRow;
	os_CurCol = curCol;
}

void os_GetCursorPos(uint24_t *curRow, uint24_t *curCol) {
	if (curRow != NULL) { *curRow = os_CurRow; }
	if (curCol != NULL) { *curCol = os_CurCol; }
}

uint24_t os_PutStrFull(const char *string) {
	str6x8(string);
	return 1;
}

uint24_t os_PutStrLine(const char *string) {
	str6x8(string);
	os_NewLine();
	return 1;
}

void os_MoveUp(void) {
	uint16_t* dst = (uint16_t*)lcd_Ram + (30 * LCD_RESX);
	const uint16_t* src = (uint16_t*)lcd_Ram + ((30 + 16) * LCD_RESX);
	for (uint8_t i = 0; i < LCD_RESY - 30 - 16; i++) {
		memcpy(dst,src,LCD_RESX * sizeof(uint16_t));
		src += LCD_RESX * sizeof(uint16_t);
		dst += LCD_RESX * sizeof(uint16_t);
	}
}

void os_MoveDown(void) {
	uint16_t* dst = (uint16_t*)lcd_Ram + (LCD_RESY * LCD_RESX);
	const uint16_t* src = (uint16_t*)lcd_Ram + ((LCD_RESY - 16) * LCD_RESX);
	for (uint8_t i = 0; i < LCD_RESY - 30 - 16; i++) {
		memcpy(dst,src,LCD_RESX * sizeof(uint16_t));
		src -= LCD_RESX * sizeof(uint16_t);
		dst -= LCD_RESX * sizeof(uint16_t);
	}
}

void os_HomeUp(void) {
	os_CurRow = 0;
	os_CurCol = 0;
}

void os_ClrLCDFull(void) {
	gColor = TiOS_WHITE;
	fillRect(0,30,LCD_RESX, LCD_RESY - 30);
}

void os_ClrLCD(void) {
	gColor = TiOS_WHITE;
	fillRect(0,30,LCD_RESX, LCD_RESY - 30);
}

void os_ClrTxtShd(void) {
	memset(RAM_ADDRESS(0xD07396),0xFF,8400); // Cmd Pixel Shadow
	memset(RAM_ADDRESS(0xD031F6),0xFF,8400); // Pixel Shadow
	memset(RAM_ADDRESS(0xD052C6),0xFF,8400); // Pixel Shadow2
}

void os_DisableHomeTextBuffer(void) {
	return;
}

void os_EnableHomeTextBuffer(void) {
	return;
}

void os_GetStringInput(const char *prompt, __attribute__((unused)) char *buf, __attribute__((unused)) size_t bufsize) {
	str6x8(prompt);
	return;
}

size_t os_GetTokenInput(const char *prompt, __attribute__((unused)) void *buf, __attribute__((unused)) size_t bufsize) {
	str6x8(prompt);
	return 0;
}


void os_FontSelect(__attribute__((unused)) font_t *font) {
	return;
}

font_t *os_FontGetID(void) {
	return NULL;
}


uint24_t os_FontGetWidth(__attribute__((unused)) const char *string) {
	return 16;
}

uint24_t os_FontGetHeight(void) {
	return 10;
}

uint24_t os_FontDrawText(const char *string, uint16_t col, uint8_t row) {
	strPos6x8(string, row, col);
	return row;
}

uint24_t os_FontDrawTransText(const char *string, uint16_t col, uint8_t row) {
	strPos6x8(string, row, col);
	return row;
}

void os_SetDrawFGColor(uint24_t color) {
	os_DrawFGColor = (uint16_t)color;
}

uint24_t os_GetDrawFGColor(void) {
	return os_DrawFGColor;
}

void os_SetDrawBGColor(uint24_t color) {
	os_DrawBGColor = (uint16_t)color;
}

uint24_t os_GetDrawBGColor(void) {
	return os_DrawBGColor;
}

int24_t os_RealToInt24(__attribute__((unused)) const real_t *arg) {
	return 0;
}

void *os_GetAnsData(__attribute__((unused)) uint8_t *type) {
	return NULL;
}
