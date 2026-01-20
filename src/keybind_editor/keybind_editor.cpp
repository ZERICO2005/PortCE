#include "keybind.h"
#include "keybind_data.h"

#include <SDL_keycode.h>
#include <SDL_scancode.h>
#include "bitGraphics.h"
#include "copyBuffer.h"

bool keyPressed(SDL_Scancode key);

/* See keybind_data.h */
enum Keyboard_Enum {KEYB_ANSI, KEYB_ANSI_Numpad, KEYB_Extended,KEYB_Extended_Numpad,KEYB_Complete,KEYB_Complete_Numpad};
const KeyBox* Keyboard_List[] = {Keyboard_ANSI,Keyboard_ANSI_Numpad,Keyboard_Extended,Keyboard_Extended_Numpad,Keyboard_Complete,Keyboard_Complete_Numpad};
const size_t Keyboard_Len[] = {ARRAY_LENGTH(Keyboard_ANSI),ARRAY_LENGTH(Keyboard_ANSI_Numpad),ARRAY_LENGTH(Keyboard_Extended),ARRAY_LENGTH(Keyboard_Extended_Numpad),ARRAY_LENGTH(Keyboard_Complete),ARRAY_LENGTH(Keyboard_Complete_Numpad)};

uint8_t Scancode_Color_Key[SDL_NUM_SCANCODES * IMAGE_BUFFER_CHANNELS];
uint8_t Scancode_Color_Hover[SDL_NUM_SCANCODES * IMAGE_BUFFER_CHANNELS];
uint8_t Scancode_Color_Press[SDL_NUM_SCANCODES * IMAGE_BUFFER_CHANNELS];
uint8_t Scancode_Color_Click[SDL_NUM_SCANCODES * IMAGE_BUFFER_CHANNELS];

static Bit_Graphics Keyboard_Graphic;
static Bit_Graphics Text_Graphic;

// Maximum dimensions needed to render key text
#define rktX 62 // 9 coloumns
#define rktY 17 // 2 rows

static void renderKeyText(
	const char* text, uint8_t cR, uint8_t cG, uint8_t cB,
	int32_t x0, int32_t y0, int32_t x1, int32_t y1
) {
	if (Text_Graphic.isInitialized() == false) { return; }
	if (x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0) { return; }
	Text_Graphic.gColor_RGB(cR,cG,cB);
	Text_Graphic.fillScreen();
	if (text != NULL) {
		Text_Graphic.gColor_RGB(0x00,0x00,0x00);
		Text_Graphic.printText6x8(0,0,text);
	}
	BufferBox src;
	BufferBox dst;
	Text_Graphic.getDrawBufferBox(&src);
	Keyboard_Graphic.getDrawBufferBox(&dst);
	if (src.vram == NULL || dst.vram == NULL) {
		if (src.vram == NULL) { fprintf(stderr, "src.vram is nullptr\n"); }
		if (dst.vram == NULL) { fprintf(stderr, "dst.vram is nullptr\n"); }
		return;
	}
	copyBuffer(src,dst,0,0,x1,y1,x0,y0,x1,y1,true);
}

// Default color

SDL_Scancode sList[] = {
	SDL_SCANCODE_ESCAPE,SDL_SCANCODE_F1,SDL_SCANCODE_F2,SDL_SCANCODE_F3,SDL_SCANCODE_F4,SDL_SCANCODE_F5,SDL_SCANCODE_F6,SDL_SCANCODE_F7,SDL_SCANCODE_F8,SDL_SCANCODE_F9,SDL_SCANCODE_F10,SDL_SCANCODE_F11,SDL_SCANCODE_F12,SDL_SCANCODE_GRAVE,SDL_SCANCODE_1,SDL_SCANCODE_2,SDL_SCANCODE_3,SDL_SCANCODE_4,SDL_SCANCODE_5,SDL_SCANCODE_6,SDL_SCANCODE_7,SDL_SCANCODE_8,SDL_SCANCODE_9,SDL_SCANCODE_0,SDL_SCANCODE_MINUS,SDL_SCANCODE_EQUALS,SDL_SCANCODE_BACKSPACE,SDL_SCANCODE_TAB,SDL_SCANCODE_Q,SDL_SCANCODE_W,SDL_SCANCODE_E,SDL_SCANCODE_R,SDL_SCANCODE_T,SDL_SCANCODE_Y,SDL_SCANCODE_U,SDL_SCANCODE_I,SDL_SCANCODE_O,SDL_SCANCODE_P,SDL_SCANCODE_LEFTBRACKET,SDL_SCANCODE_RIGHTBRACKET,SDL_SCANCODE_BACKSLASH,SDL_SCANCODE_CAPSLOCK,SDL_SCANCODE_A,SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_F,SDL_SCANCODE_G,SDL_SCANCODE_H,SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_L,SDL_SCANCODE_SEMICOLON,SDL_SCANCODE_APOSTROPHE,SDL_SCANCODE_RETURN,SDL_SCANCODE_LSHIFT,SDL_SCANCODE_Z,SDL_SCANCODE_X,SDL_SCANCODE_C,SDL_SCANCODE_V,SDL_SCANCODE_B,SDL_SCANCODE_N,SDL_SCANCODE_M,SDL_SCANCODE_COMMA,SDL_SCANCODE_PERIOD,SDL_SCANCODE_SLASH,SDL_SCANCODE_RSHIFT,SDL_SCANCODE_LCTRL,SDL_SCANCODE_LGUI,SDL_SCANCODE_LALT,SDL_SCANCODE_SPACE,SDL_SCANCODE_RALT,SDL_SCANCODE_RGUI,SDL_SCANCODE_APPLICATION,SDL_SCANCODE_RCTRL,SDL_SCANCODE_PRINTSCREEN,SDL_SCANCODE_SCROLLLOCK,SDL_SCANCODE_PAUSE,SDL_SCANCODE_INSERT,SDL_SCANCODE_HOME,SDL_SCANCODE_PAGEUP,SDL_SCANCODE_DELETE,SDL_SCANCODE_END,SDL_SCANCODE_PAGEDOWN,SDL_SCANCODE_UP,SDL_SCANCODE_LEFT,SDL_SCANCODE_DOWN,SDL_SCANCODE_RIGHT,SDL_SCANCODE_NUMLOCKCLEAR,SDL_SCANCODE_KP_DIVIDE,SDL_SCANCODE_KP_MULTIPLY,SDL_SCANCODE_KP_MINUS,SDL_SCANCODE_KP_7,SDL_SCANCODE_KP_8,SDL_SCANCODE_KP_9,SDL_SCANCODE_KP_PLUS,SDL_SCANCODE_KP_4,SDL_SCANCODE_KP_5,SDL_SCANCODE_KP_6,SDL_SCANCODE_KP_1,SDL_SCANCODE_KP_2,SDL_SCANCODE_KP_3,SDL_SCANCODE_KP_ENTER,SDL_SCANCODE_KP_0,SDL_SCANCODE_KP_PERIOD,SDL_SCANCODE_EJECT,SDL_SCANCODE_UNDO,SDL_SCANCODE_CUT,SDL_SCANCODE_COPY,SDL_SCANCODE_PASTE,SDL_SCANCODE_AUDIOMUTE,SDL_SCANCODE_VOLUMEDOWN,SDL_SCANCODE_VOLUMEUP,SDL_SCANCODE_MUTE,SDL_SCANCODE_MEDIASELECT,SDL_SCANCODE_BRIGHTNESSDOWN,SDL_SCANCODE_NONUSBACKSLASH,SDL_SCANCODE_HELP,SDL_SCANCODE_MENU,SDL_SCANCODE_AGAIN,SDL_SCANCODE_FIND,SDL_SCANCODE_SELECT,SDL_SCANCODE_RETURN2,SDL_SCANCODE_AUDIOPREV,SDL_SCANCODE_AUDIOPLAY,SDL_SCANCODE_AUDIONEXT,SDL_SCANCODE_AUDIOSTOP,SDL_SCANCODE_MAIL,SDL_SCANCODE_BRIGHTNESSUP,SDL_SCANCODE_NONUSHASH,SDL_SCANCODE_WWW,SDL_SCANCODE_SYSREQ,SDL_SCANCODE_SLEEP,SDL_SCANCODE_POWER,SDL_SCANCODE_CURRENCYUNIT,SDL_SCANCODE_CURRENCYSUBUNIT,SDL_SCANCODE_SEPARATOR,SDL_SCANCODE_DECIMALSEPARATOR,SDL_SCANCODE_THOUSANDSSEPARATOR,SDL_SCANCODE_KP_MEMRECALL,SDL_SCANCODE_KP_MEMSTORE,SDL_SCANCODE_KP_MEMCLEAR,SDL_SCANCODE_KP_BACKSPACE,SDL_SCANCODE_KP_MEMADD,SDL_SCANCODE_KP_MEMSUBTRACT,SDL_SCANCODE_KP_DIVIDE,SDL_SCANCODE_KP_MULTIPLY,SDL_SCANCODE_KP_CLEAR,SDL_SCANCODE_KP_7,SDL_SCANCODE_KP_8,SDL_SCANCODE_KP_9,SDL_SCANCODE_KP_MINUS,SDL_SCANCODE_KP_CLEARENTRY,SDL_SCANCODE_KP_4,SDL_SCANCODE_KP_5,SDL_SCANCODE_KP_6,SDL_SCANCODE_KP_PLUS,SDL_SCANCODE_KP_PLUSMINUS,SDL_SCANCODE_KP_1,SDL_SCANCODE_KP_2,SDL_SCANCODE_KP_3,SDL_SCANCODE_KP_PERCENT,SDL_SCANCODE_KP_0,SDL_SCANCODE_KP_PERIOD,SDL_SCANCODE_KP_ENTER,SDL_SCANCODE_KP_POWER,SDL_SCANCODE_KP_00,SDL_SCANCODE_KP_000,SDL_SCANCODE_KP_EQUALS,SDL_SCANCODE_NUMLOCKCLEAR,SDL_SCANCODE_EJECT,SDL_SCANCODE_F13,SDL_SCANCODE_F14,SDL_SCANCODE_F15,SDL_SCANCODE_F16,SDL_SCANCODE_F17,SDL_SCANCODE_F18,SDL_SCANCODE_F19,SDL_SCANCODE_F20,SDL_SCANCODE_F21,SDL_SCANCODE_F22,SDL_SCANCODE_F23,SDL_SCANCODE_F24,SDL_SCANCODE_SYSREQ,SDL_SCANCODE_SLEEP,SDL_SCANCODE_POWER,SDL_SCANCODE_CLEAR,SDL_SCANCODE_AC_HOME,SDL_SCANCODE_AC_REFRESH,SDL_SCANCODE_AC_STOP,SDL_SCANCODE_AC_BOOKMARKS,SDL_SCANCODE_AUDIOMUTE,SDL_SCANCODE_VOLUMEDOWN,SDL_SCANCODE_VOLUMEUP,SDL_SCANCODE_MUTE,SDL_SCANCODE_KBDILLUMDOWN,SDL_SCANCODE_KBDILLUMUP,SDL_SCANCODE_KBDILLUMTOGGLE,SDL_SCANCODE_CLEARAGAIN,SDL_SCANCODE_AC_BACK,SDL_SCANCODE_AC_SEARCH,SDL_SCANCODE_AC_FORWARD,SDL_SCANCODE_MEDIASELECT,SDL_SCANCODE_AUDIOPREV,SDL_SCANCODE_AUDIOPLAY,SDL_SCANCODE_AUDIONEXT,SDL_SCANCODE_AUDIOSTOP,SDL_SCANCODE_BRIGHTNESSDOWN,SDL_SCANCODE_BRIGHTNESSUP,SDL_SCANCODE_HELP,SDL_SCANCODE_LOCKINGCAPSLOCK,SDL_SCANCODE_LOCKINGNUMLOCK,SDL_SCANCODE_LOCKINGSCROLLLOCK,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_CRSEL,SDL_SCANCODE_ALTERASE,SDL_SCANCODE_EXECUTE,SDL_SCANCODE_STOP,SDL_SCANCODE_MENU,SDL_SCANCODE_OUT,SDL_SCANCODE_EXSEL,SDL_SCANCODE_PRIOR,SDL_SCANCODE_OPER,SDL_SCANCODE_CANCEL,SDL_SCANCODE_UNDO,SDL_SCANCODE_CUT,SDL_SCANCODE_COPY,SDL_SCANCODE_PASTE,SDL_SCANCODE_CALCULATOR,SDL_SCANCODE_MODE,SDL_SCANCODE_MAIL,SDL_SCANCODE_NONUSBACKSLASH,SDL_SCANCODE_AGAIN,SDL_SCANCODE_FIND,SDL_SCANCODE_SELECT,SDL_SCANCODE_RETURN2,SDL_SCANCODE_COMPUTER,SDL_SCANCODE_DISPLAYSWITCH,SDL_SCANCODE_WWW,SDL_SCANCODE_NONUSHASH,SDL_SCANCODE_CURRENCYUNIT,SDL_SCANCODE_CURRENCYSUBUNIT,SDL_SCANCODE_SEPARATOR,SDL_SCANCODE_DECIMALSEPARATOR,SDL_SCANCODE_THOUSANDSSEPARATOR,SDL_SCANCODE_INTERNATIONAL1,SDL_SCANCODE_INTERNATIONAL2,SDL_SCANCODE_INTERNATIONAL3,SDL_SCANCODE_INTERNATIONAL4,SDL_SCANCODE_INTERNATIONAL5,SDL_SCANCODE_INTERNATIONAL6,SDL_SCANCODE_INTERNATIONAL7,SDL_SCANCODE_INTERNATIONAL8,SDL_SCANCODE_INTERNATIONAL9,SDL_SCANCODE_LANG1,SDL_SCANCODE_LANG2,SDL_SCANCODE_LANG3,SDL_SCANCODE_LANG4,SDL_SCANCODE_LANG5,SDL_SCANCODE_LANG6,SDL_SCANCODE_LANG7,SDL_SCANCODE_LANG8,SDL_SCANCODE_LANG9,SDL_SCANCODE_KP_CLEARENTRY,SDL_SCANCODE_KP_CLEARENTRY,SDL_SCANCODE_KP_BACKSPACE,SDL_SCANCODE_KP_DIVIDE,SDL_SCANCODE_KP_PERCENT,SDL_SCANCODE_KP_D,SDL_SCANCODE_KP_E,SDL_SCANCODE_KP_F,SDL_SCANCODE_KP_MULTIPLY,SDL_SCANCODE_KP_POWER,SDL_SCANCODE_KP_A,SDL_SCANCODE_KP_B,SDL_SCANCODE_KP_C,SDL_SCANCODE_KP_7,SDL_SCANCODE_KP_8,SDL_SCANCODE_KP_9,SDL_SCANCODE_KP_MINUS,SDL_SCANCODE_KP_PLUSMINUS,SDL_SCANCODE_KP_4,SDL_SCANCODE_KP_5,SDL_SCANCODE_KP_6,SDL_SCANCODE_KP_PLUS,SDL_SCANCODE_KP_DECIMAL,SDL_SCANCODE_KP_1,SDL_SCANCODE_KP_2,SDL_SCANCODE_KP_3,SDL_SCANCODE_KP_HEXADECIMAL,SDL_SCANCODE_KP_0,SDL_SCANCODE_KP_PERIOD,SDL_SCANCODE_KP_ENTER,SDL_SCANCODE_KP_OCTAL,SDL_SCANCODE_KP_00,SDL_SCANCODE_KP_000,SDL_SCANCODE_KP_EQUALS,SDL_SCANCODE_KP_BINARY,SDL_SCANCODE_KP_MEMRECALL,SDL_SCANCODE_NUMLOCKCLEAR,SDL_SCANCODE_KP_MEMDIVIDE,SDL_SCANCODE_KP_MEMSTORE,SDL_SCANCODE_KP_MEMCLEAR,SDL_SCANCODE_KP_EQUALSAS400,SDL_SCANCODE_KP_MEMMULTIPLY,SDL_SCANCODE_KP_LEFTPAREN,SDL_SCANCODE_KP_RIGHTPAREN,SDL_SCANCODE_KP_AT,SDL_SCANCODE_KP_MEMSUBTRACT,SDL_SCANCODE_KP_LEFTBRACE,SDL_SCANCODE_KP_RIGHTBRACE,SDL_SCANCODE_KP_COLON,SDL_SCANCODE_KP_MEMADD,SDL_SCANCODE_KP_LESS,SDL_SCANCODE_KP_GREATER,SDL_SCANCODE_KP_HASH,SDL_SCANCODE_KP_AMPERSAND,SDL_SCANCODE_KP_DBLAMPERSAND,SDL_SCANCODE_KP_SPACE,SDL_SCANCODE_KP_VERTICALBAR,SDL_SCANCODE_KP_DBLVERTICALBAR,SDL_SCANCODE_KP_TAB,SDL_SCANCODE_KP_XOR,SDL_SCANCODE_KP_EXCLAM,SDL_SCANCODE_KP_COMMA
};
const char* nList[] = {
	"SDL_SCANCODE_ESCAPE","SDL_SCANCODE_F1","SDL_SCANCODE_F2","SDL_SCANCODE_F3","SDL_SCANCODE_F4","SDL_SCANCODE_F5","SDL_SCANCODE_F6","SDL_SCANCODE_F7","SDL_SCANCODE_F8","SDL_SCANCODE_F9","SDL_SCANCODE_F10","SDL_SCANCODE_F11","SDL_SCANCODE_F12","SDL_SCANCODE_GRAVE","SDL_SCANCODE_1","SDL_SCANCODE_2","SDL_SCANCODE_3","SDL_SCANCODE_4","SDL_SCANCODE_5","SDL_SCANCODE_6","SDL_SCANCODE_7","SDL_SCANCODE_8","SDL_SCANCODE_9","SDL_SCANCODE_0","SDL_SCANCODE_MINUS","SDL_SCANCODE_EQUALS","SDL_SCANCODE_BACKSPACE","SDL_SCANCODE_TAB","SDL_SCANCODE_Q","SDL_SCANCODE_W","SDL_SCANCODE_E","SDL_SCANCODE_R","SDL_SCANCODE_T","SDL_SCANCODE_Y","SDL_SCANCODE_U","SDL_SCANCODE_I","SDL_SCANCODE_O","SDL_SCANCODE_P","SDL_SCANCODE_LEFTBRACKET","SDL_SCANCODE_RIGHTBRACKET","SDL_SCANCODE_BACKSLASH","SDL_SCANCODE_CAPSLOCK","SDL_SCANCODE_A","SDL_SCANCODE_S","SDL_SCANCODE_D","SDL_SCANCODE_F","SDL_SCANCODE_G","SDL_SCANCODE_H","SDL_SCANCODE_J","SDL_SCANCODE_K","SDL_SCANCODE_L","SDL_SCANCODE_SEMICOLON","SDL_SCANCODE_APOSTROPHE","SDL_SCANCODE_RETURN","SDL_SCANCODE_LSHIFT","SDL_SCANCODE_Z","SDL_SCANCODE_X","SDL_SCANCODE_C","SDL_SCANCODE_V","SDL_SCANCODE_B","SDL_SCANCODE_N","SDL_SCANCODE_M","SDL_SCANCODE_COMMA","SDL_SCANCODE_PERIOD","SDL_SCANCODE_SLASH","SDL_SCANCODE_RSHIFT","SDL_SCANCODE_LCTRL","SDL_SCANCODE_LGUI","SDL_SCANCODE_LALT","SDL_SCANCODE_SPACE","SDL_SCANCODE_RALT","SDL_SCANCODE_RGUI","SDL_SCANCODE_APPLICATION","SDL_SCANCODE_RCTRL","SDL_SCANCODE_PRINTSCREEN","SDL_SCANCODE_SCROLLLOCK","SDL_SCANCODE_PAUSE","SDL_SCANCODE_INSERT","SDL_SCANCODE_HOME","SDL_SCANCODE_PAGEUP","SDL_SCANCODE_DELETE","SDL_SCANCODE_END","SDL_SCANCODE_PAGEDOWN","SDL_SCANCODE_UP","SDL_SCANCODE_LEFT","SDL_SCANCODE_DOWN","SDL_SCANCODE_RIGHT","SDL_SCANCODE_NUMLOCKCLEAR","SDL_SCANCODE_KP_DIVIDE","SDL_SCANCODE_KP_MULTIPLY","SDL_SCANCODE_KP_MINUS","SDL_SCANCODE_KP_7","SDL_SCANCODE_KP_8","SDL_SCANCODE_KP_9","SDL_SCANCODE_KP_PLUS","SDL_SCANCODE_KP_4","SDL_SCANCODE_KP_5","SDL_SCANCODE_KP_6","SDL_SCANCODE_KP_1","SDL_SCANCODE_KP_2","SDL_SCANCODE_KP_3","SDL_SCANCODE_KP_ENTER","SDL_SCANCODE_KP_0","SDL_SCANCODE_KP_PERIOD","SDL_SCANCODE_EJECT","SDL_SCANCODE_UNDO","SDL_SCANCODE_CUT","SDL_SCANCODE_COPY","SDL_SCANCODE_PASTE","SDL_SCANCODE_AUDIOMUTE","SDL_SCANCODE_VOLUMEDOWN","SDL_SCANCODE_VOLUMEUP","SDL_SCANCODE_MUTE","SDL_SCANCODE_MEDIASELECT","SDL_SCANCODE_BRIGHTNESSDOWN","SDL_SCANCODE_NONUSBACKSLASH","SDL_SCANCODE_HELP","SDL_SCANCODE_MENU","SDL_SCANCODE_AGAIN","SDL_SCANCODE_FIND","SDL_SCANCODE_SELECT","SDL_SCANCODE_RETURN2","SDL_SCANCODE_AUDIOPREV","SDL_SCANCODE_AUDIOPLAY","SDL_SCANCODE_AUDIONEXT","SDL_SCANCODE_AUDIOSTOP","SDL_SCANCODE_MAIL","SDL_SCANCODE_BRIGHTNESSUP","SDL_SCANCODE_NONUSHASH","SDL_SCANCODE_WWW","SDL_SCANCODE_SYSREQ","SDL_SCANCODE_SLEEP","SDL_SCANCODE_POWER","SDL_SCANCODE_CURRENCYUNIT","SDL_SCANCODE_CURRENCYSUBUNIT","SDL_SCANCODE_SEPARATOR","SDL_SCANCODE_DECIMALSEPARATOR","SDL_SCANCODE_THOUSANDSSEPARATOR","SDL_SCANCODE_KP_MEMRECALL","SDL_SCANCODE_KP_MEMSTORE","SDL_SCANCODE_KP_MEMCLEAR","SDL_SCANCODE_KP_BACKSPACE","SDL_SCANCODE_KP_MEMADD","SDL_SCANCODE_KP_MEMSUBTRACT","SDL_SCANCODE_KP_DIVIDE","SDL_SCANCODE_KP_MULTIPLY","SDL_SCANCODE_KP_CLEAR","SDL_SCANCODE_KP_7","SDL_SCANCODE_KP_8","SDL_SCANCODE_KP_9","SDL_SCANCODE_KP_MINUS","SDL_SCANCODE_KP_CLEARENTRY","SDL_SCANCODE_KP_4","SDL_SCANCODE_KP_5","SDL_SCANCODE_KP_6","SDL_SCANCODE_KP_PLUS","SDL_SCANCODE_KP_PLUSMINUS","SDL_SCANCODE_KP_1","SDL_SCANCODE_KP_2","SDL_SCANCODE_KP_3","SDL_SCANCODE_KP_PERCENT","SDL_SCANCODE_KP_0","SDL_SCANCODE_KP_PERIOD","SDL_SCANCODE_KP_ENTER","SDL_SCANCODE_KP_POWER","SDL_SCANCODE_KP_00","SDL_SCANCODE_KP_000","SDL_SCANCODE_KP_EQUALS","SDL_SCANCODE_NUMLOCKCLEAR","SDL_SCANCODE_EJECT","SDL_SCANCODE_F13","SDL_SCANCODE_F14","SDL_SCANCODE_F15","SDL_SCANCODE_F16","SDL_SCANCODE_F17","SDL_SCANCODE_F18","SDL_SCANCODE_F19","SDL_SCANCODE_F20","SDL_SCANCODE_F21","SDL_SCANCODE_F22","SDL_SCANCODE_F23","SDL_SCANCODE_F24","SDL_SCANCODE_SYSREQ","SDL_SCANCODE_SLEEP","SDL_SCANCODE_POWER","SDL_SCANCODE_CLEAR","SDL_SCANCODE_AC_HOME","SDL_SCANCODE_AC_REFRESH","SDL_SCANCODE_AC_STOP","SDL_SCANCODE_AC_BOOKMARKS","SDL_SCANCODE_AUDIOMUTE","SDL_SCANCODE_VOLUMEDOWN","SDL_SCANCODE_VOLUMEUP","SDL_SCANCODE_MUTE","SDL_SCANCODE_KBDILLUMDOWN","SDL_SCANCODE_KBDILLUMUP","SDL_SCANCODE_KBDILLUMTOGGLE","SDL_SCANCODE_CLEARAGAIN","SDL_SCANCODE_AC_BACK","SDL_SCANCODE_AC_SEARCH","SDL_SCANCODE_AC_FORWARD","SDL_SCANCODE_MEDIASELECT","SDL_SCANCODE_AUDIOPREV","SDL_SCANCODE_AUDIOPLAY","SDL_SCANCODE_AUDIONEXT","SDL_SCANCODE_AUDIOSTOP","SDL_SCANCODE_BRIGHTNESSDOWN","SDL_SCANCODE_BRIGHTNESSUP","SDL_SCANCODE_HELP","SDL_SCANCODE_LOCKINGCAPSLOCK","SDL_SCANCODE_LOCKINGNUMLOCK","SDL_SCANCODE_LOCKINGSCROLLLOCK","SDL_SCANCODE_UNKNOWN","SDL_SCANCODE_CRSEL","SDL_SCANCODE_ALTERASE","SDL_SCANCODE_EXECUTE","SDL_SCANCODE_STOP","SDL_SCANCODE_MENU","SDL_SCANCODE_OUT","SDL_SCANCODE_EXSEL","SDL_SCANCODE_PRIOR","SDL_SCANCODE_OPER","SDL_SCANCODE_CANCEL","SDL_SCANCODE_UNDO","SDL_SCANCODE_CUT","SDL_SCANCODE_COPY","SDL_SCANCODE_PASTE","SDL_SCANCODE_CALCULATOR","SDL_SCANCODE_MODE","SDL_SCANCODE_MAIL","SDL_SCANCODE_NONUSBACKSLASH","SDL_SCANCODE_AGAIN","SDL_SCANCODE_FIND","SDL_SCANCODE_SELECT","SDL_SCANCODE_RETURN2","SDL_SCANCODE_COMPUTER","SDL_SCANCODE_DISPLAYSWITCH","SDL_SCANCODE_WWW","SDL_SCANCODE_NONUSHASH","SDL_SCANCODE_CURRENCYUNIT","SDL_SCANCODE_CURRENCYSUBUNIT","SDL_SCANCODE_SEPARATOR","SDL_SCANCODE_DECIMALSEPARATOR","SDL_SCANCODE_THOUSANDSSEPARATOR","SDL_SCANCODE_INTERNATIONAL1","SDL_SCANCODE_INTERNATIONAL2","SDL_SCANCODE_INTERNATIONAL3","SDL_SCANCODE_INTERNATIONAL4","SDL_SCANCODE_INTERNATIONAL5","SDL_SCANCODE_INTERNATIONAL6","SDL_SCANCODE_INTERNATIONAL7","SDL_SCANCODE_INTERNATIONAL8","SDL_SCANCODE_INTERNATIONAL9","SDL_SCANCODE_LANG1","SDL_SCANCODE_LANG2","SDL_SCANCODE_LANG3","SDL_SCANCODE_LANG4","SDL_SCANCODE_LANG5","SDL_SCANCODE_LANG6","SDL_SCANCODE_LANG7","SDL_SCANCODE_LANG8","SDL_SCANCODE_LANG9","SDL_SCANCODE_KP_CLEARENTRY","SDL_SCANCODE_KP_CLEARENTRY","SDL_SCANCODE_KP_BACKSPACE","SDL_SCANCODE_KP_DIVIDE","SDL_SCANCODE_KP_PERCENT","SDL_SCANCODE_KP_D","SDL_SCANCODE_KP_E","SDL_SCANCODE_KP_F","SDL_SCANCODE_KP_MULTIPLY","SDL_SCANCODE_KP_POWER","SDL_SCANCODE_KP_A","SDL_SCANCODE_KP_B","SDL_SCANCODE_KP_C","SDL_SCANCODE_KP_7","SDL_SCANCODE_KP_8","SDL_SCANCODE_KP_9","SDL_SCANCODE_KP_MINUS","SDL_SCANCODE_KP_PLUSMINUS","SDL_SCANCODE_KP_4","SDL_SCANCODE_KP_5","SDL_SCANCODE_KP_6","SDL_SCANCODE_KP_PLUS","SDL_SCANCODE_KP_DECIMAL","SDL_SCANCODE_KP_1","SDL_SCANCODE_KP_2","SDL_SCANCODE_KP_3","SDL_SCANCODE_KP_HEXADECIMAL","SDL_SCANCODE_KP_0","SDL_SCANCODE_KP_PERIOD","SDL_SCANCODE_KP_ENTER","SDL_SCANCODE_KP_OCTAL","SDL_SCANCODE_KP_00","SDL_SCANCODE_KP_000","SDL_SCANCODE_KP_EQUALS","SDL_SCANCODE_KP_BINARY","SDL_SCANCODE_KP_MEMRECALL","SDL_SCANCODE_NUMLOCKCLEAR","SDL_SCANCODE_KP_MEMDIVIDE","SDL_SCANCODE_KP_MEMSTORE","SDL_SCANCODE_KP_MEMCLEAR","SDL_SCANCODE_KP_EQUALSAS400","SDL_SCANCODE_KP_MEMMULTIPLY","SDL_SCANCODE_KP_LEFTPAREN","SDL_SCANCODE_KP_RIGHTPAREN","SDL_SCANCODE_KP_AT","SDL_SCANCODE_KP_MEMSUBTRACT","SDL_SCANCODE_KP_LEFTBRACE","SDL_SCANCODE_KP_RIGHTBRACE","SDL_SCANCODE_KP_COLON","SDL_SCANCODE_KP_MEMADD","SDL_SCANCODE_KP_LESS","SDL_SCANCODE_KP_GREATER","SDL_SCANCODE_KP_HASH","SDL_SCANCODE_KP_AMPERSAND","SDL_SCANCODE_KP_DBLAMPERSAND","SDL_SCANCODE_KP_SPACE","SDL_SCANCODE_KP_VERTICALBAR","SDL_SCANCODE_KP_DBLVERTICALBAR","SDL_SCANCODE_KP_TAB","SDL_SCANCODE_KP_XOR","SDL_SCANCODE_KP_EXCLAM","SDL_SCANCODE_KP_COMMA"
};

void initKeyboardGraphics(double hue, double sat, double val) {
	/*
	printf("\n\nconst char* const Scancode_Name[] = {");
	for (size_t a = 0; a < SDL_NUM_SCANCODES; a++) {
		if (a % 16 == 0) {
			printf("\n\t");
			fflush(stdout);
		}
		size_t b = 0;
		for (; b < ARRAY_LENGTH(sList); b++) {
			if (sList[b] == a) {
				if (sList[b] == SDL_SCANCODE_UNKNOWN) {
					printf("\"SDL_SCANCODE_UNKNOWN\",");
				} else {
					printf("\"%s\",",nList[b]);
				}
				break;
			}
		}
		if (b == ARRAY_LENGTH(sList)) {
			printf("\"SDL_SCANCODE_UNKNOWN\",");
		}
	}
	printf("\b};\n\n");
	fflush(stdout);
	*/
	if (Text_Graphic.isInitialized() == false) {
		Text_Graphic = Bit_Graphics(rktX,rktY);
		if (Text_Graphic.isInitialized() == false) {
			fprintf(stderr, "Text_Graphic failed to initialize");
			return;
		}
	}
	uint8_t rK = 0, gK = 0, bK = 0;
	uint8_t rH = 0, gH = 0, bH = 0;
	uint8_t rP = 0, gP = 0, bP = 0;
	uint8_t rC = 0, gC = 0, bC = 0;
	getRGBfromHSV(&rK,&gK,&bK,hue,sat,val);
	getRGBfromHSV(&rH,&gH,&bH,hue,sat,val * 0.8333);
	getRGBfromHSV(&rP,&gP,&bP,hue,sat,val * 0.75);
	getRGBfromHSV(&rC,&gC,&bC,hue,sat,val * 0.6667);
	size_t z = 0;
	for (size_t i = 0; i < SDL_NUM_SCANCODES; i++) {
		Scancode_Color_Key[z] = rK;
		Scancode_Color_Hover[z] = rH;
		Scancode_Color_Press[z] = rP;
		Scancode_Color_Click[z] = rC;
		z++;
		Scancode_Color_Key[z] = gK;
		Scancode_Color_Hover[z] = gH;
		Scancode_Color_Press[z] = gP;
		Scancode_Color_Click[z] = gC;
		z++;
		Scancode_Color_Key[z] = bK;
		Scancode_Color_Hover[z] = bH;
		Scancode_Color_Press[z] = bP;
		Scancode_Color_Click[z] = bC;
		z++;
		if (IMAGE_BUFFER_CHANNELS == 4) {
			Scancode_Color_Key[z] = 0xFF;
			Scancode_Color_Hover[z] = 0xFF;
			Scancode_Color_Press[z] = 0xFF;
			Scancode_Color_Click[z] = 0xFF;
			z++;
		}
	}
}
void terminateKeyboardGraphics() {
	Keyboard_Graphic.terminate_Bit_Graphics();
	Text_Graphic.terminate_Bit_Graphics();
}

static void calcBoardBounds(
	enum Keyboard_Enum board,
	size_t& keyCount,
	int32_t& minDimX, int32_t& minDimY,
	int32_t& maxDimX, int32_t& maxDimY
) {
	for (size_t i = 0; i < Keyboard_Len[board]; i++) {
		int32_t minPosX = Keyboard_List[board][i].posX;
		int32_t maxPosX = Keyboard_List[board][i].posX + Keyboard_List[board][i].sizeX;
		int32_t minPosY = Keyboard_List[board][i].posY;
		int32_t maxPosY = Keyboard_List[board][i].posY + Keyboard_List[board][i].sizeY;
		minDimX = (minPosX < minDimX) ? minPosX : minDimX;
		maxDimX = (maxPosX > maxDimX) ? maxPosX : maxDimX;
		minDimY = (minPosY < minDimY) ? minPosY : minDimY;
		maxDimY = (maxPosY > maxDimY) ? maxPosY : maxDimY;
		keyCount++;
	}
}

void renderBoard(
	enum Keyboard_Enum board,
	int32_t& curX, int32_t& curY, bool& click, SDL_Scancode* KB_scancode, SDL_KeyCode* KB_keycode,
	int32_t offsetX, int32_t offsetY,
	const int32_t& KeyboardBorder, const int32_t& KeySpacing,
	const double& keyScaleX, const double& keyScaleY
) {
	for (size_t i = 0; i < Keyboard_Len[board]; i++) {
		const KeyBox& k = Keyboard_List[board][i];
		int32_t x0 = (int32_t)((double)(k.posX + offsetX) * keyScaleX);
		int32_t y0 = (int32_t)((double)(k.posY + offsetY) * keyScaleY);
		int32_t x1 = (int32_t)((double)(k.posX + offsetX + k.sizeX) * keyScaleX) - x0;
		int32_t y1 = (int32_t)((double)(k.posY + offsetY + k.sizeY) * keyScaleY) - y0;
		x0 += KeyboardBorder; y0 += KeyboardBorder;
		x1 -= KeySpacing; y1 -= KeySpacing;
		if (x1 >= 3 && y1 >= 3) { /* Renders Key only if it is large enough */
			size_t pos = (size_t)k.Scancode * IMAGE_BUFFER_CHANNELS;
			uint8_t* col = Scancode_Color_Key;
			if ((curX >= x0 && curX <= x0 + x1) && (curY >= y0 && curY <= y0 + y1)) {
				col = click ? Scancode_Color_Click : Scancode_Color_Hover;
				if (KB_scancode != nullptr) { *KB_scancode = k.Scancode; }
				if (KB_keycode != nullptr) { *KB_keycode = k.Keycode; }
			}
			if (keyPressed(k.Scancode) && col != Scancode_Color_Click) {
				col = Scancode_Color_Press;
			}
			Keyboard_Graphic.gColor_RGB(0x40,0x40,0x40);
			Keyboard_Graphic.drawRect((size_t)(int64_t)x0,(size_t)(int64_t)y0,(size_t)(int64_t)x1,(size_t)(int64_t)y1);
			Keyboard_Graphic.gColor_RGB(col[pos], col[pos + 1], col[pos + 2]);
			Keyboard_Graphic.fillRect((size_t)(int64_t)(x0+1),(size_t)(int64_t)(y0+1),(size_t)(int64_t)(x1-2),(size_t)(int64_t)(y1-2));
			renderKeyText(
				Keyboard_List[board][i].name,
				col[pos],col[pos + 1],col[pos + 2],
				x0 + 2, y0 + 2, x1 - 4, y1 - 4
			);
			/* Keyboard_Graphic.printText6x8(x0+2,y0+2,(char*)Keyboard_List[b][i].name); */
		}
	}
}

void renderKeyboard(
	BufferBox* buf, int32_t ResX, double minScaleX, double maxScaleX,
	uint8_t KeyboardSize, bool includeNumpad,
	int32_t curX, int32_t curY, bool click, SDL_Scancode* KB_scancode, SDL_KeyCode* KB_keycode, bool* hoverInBounds
) {
	// nano64_t startTime = getNanoTime();

	if (buf == NULL) { return; }
	initBufferBox(buf,NULL,0,0,IMAGE_BUFFER_CHANNELS,0);
	if (KeyboardSize >= Number_of_Keyboard_Sizes) { return; }
	if (maxScaleX <= 0.0 || minScaleX <= 0.0) { return; }
	/* Hardcoded Values */
		const int32_t KeyboardBorder = 4;
		const int32_t KeySpacing = 2;
	int32_t offsetVal = (KeyboardBorder * 2) - KeySpacing;
	if (offsetVal < 0) { return; } // Negative offset value
	if (ResX < offsetVal) { return; } // Unable to meet

	int32_t offsetX = 0;
	int32_t offsetY = 0;
	int32_t dimX = 0;
	int32_t dimY = 0;
	int32_t minDimX = INT32_MAX;
	int32_t minDimY = INT32_MAX;
	int32_t maxDimX = INT32_MIN;
	int32_t maxDimY = INT32_MIN;

	size_t keyCount = 0;

	/* Calculate the size of the keyboard */
	calcBoardBounds(
		KEYB_ANSI,
		keyCount,
		minDimX, minDimY,
		maxDimX, maxDimY
	);
	if (includeNumpad == true) {
		calcBoardBounds(
			(Keyboard_Enum)(KeyboardSize * 2 + 1),
			keyCount,
			minDimX, minDimY,
			maxDimX, maxDimY
		);
	}
	if (KeyboardSize >= 1) { /* Extended and Complete keyboards */
		calcBoardBounds(
			(Keyboard_Enum)(KeyboardSize * 2),
			keyCount,
			minDimX, minDimY,
			maxDimX, maxDimY
		);
	}
	if (maxDimX == INT32_MIN || maxDimY == INT32_MIN || minDimX == INT32_MAX || minDimY == INT32_MAX) {
		fprintf(stderr, "Unable to get Keyboard_Graphic dimensions");
		return;
	}

	dimX = maxDimX - minDimX;
	dimY = maxDimY - minDimY;
	offsetX = -minDimX;
	offsetY = -minDimY;

	int32_t resX = ResX - offsetVal;
	double keyScaleX = (double)(resX) / (double)dimX;
	if (keyScaleX > maxScaleX) {
		resX = (int32_t)(maxScaleX * (double)dimX);
		keyScaleX = (double)(resX) / (double)dimX;
	} else if (keyScaleX < minScaleX) {
		resX = (int32_t)(minScaleX * (double)dimX);
		keyScaleX = (double)(resX) / (double)dimX;
	}
	int32_t resY = (int32_t)((double)resX * ((double)dimY / (double)dimX));
	double keyScaleY = (double)(resY) / (double)dimY;

	buf->resX = resX + offsetVal;
	buf->resY = resY + offsetVal;

	//printfInterval(0.6,"\nRes{%ux%u}\nScales: %.3lf %.3lf | %.3lf",buf->resX,buf->resY,keyScaleX,keyScaleY,keyScaleX/keyScaleY);
	//printfInterval(0.5,"\n%zu dim{%d,%d} min{%d,%d} max{%d,%d} offset{%d,%d}",keyCount,dimX,dimY,minDimX,minDimY,maxDimX,maxDimY,offsetX,offsetY);

	/* Update Keyboard_Graphic buffers */
	if (Keyboard_Graphic.isInitialized() == true) {
		Keyboard_Graphic.resizeBuffer((size_t)buf->resX,(size_t)buf->resY);
		if (Keyboard_Graphic.isInitialized() == false) {
			fprintf(stderr, "Failed to reallocate buffers for Keyboard_Graphic");
			return;
		}
	} else {
		Keyboard_Graphic = Bit_Graphics((size_t)buf->resX,(size_t)buf->resY);
		if (Keyboard_Graphic.isInitialized() == false) {
			fprintf(stderr, "Keyboard_Graphic failed to initialize");
			return;
		}
	}

	if (KB_scancode != nullptr) { *KB_scancode = SDL_SCANCODE_UNKNOWN; }
	if (KB_keycode != nullptr) { *KB_keycode = SDLK_UNKNOWN; }

	Keyboard_Graphic.gColor_RGB_hex(0xC0C0C0);
	Keyboard_Graphic.fillScreen();

	renderBoard(
		KEYB_ANSI,
		curX, curY,
		click, KB_scancode, KB_keycode,
		offsetX, offsetY,
		KeyboardBorder, KeySpacing,
		keyScaleX, keyScaleY
	);
	if (includeNumpad == true) {
		renderBoard(
			(Keyboard_Enum)(KeyboardSize * 2 + 1),
			curX, curY,
			click, KB_scancode, KB_keycode,
			offsetX, offsetY,
			KeyboardBorder, KeySpacing,
			keyScaleX, keyScaleY
		);
	}
	if (KeyboardSize >= 1) { /* Extended and Complete keyboards */
		renderBoard(
			(Keyboard_Enum)(KeyboardSize * 2),
			curX, curY,
			click, KB_scancode, KB_keycode,
			offsetX, offsetY,
			KeyboardBorder, KeySpacing,
			keyScaleX, keyScaleY
		);
	}

	if (curX >= 0 && curX < (int32_t)buf->resX && curY >= 0 && curY < (int32_t)buf->resY) {
		*hoverInBounds = true;
	} else {
		*hoverInBounds = false;
	}

	Keyboard_Graphic.swapBuffer();
	buf->vram = reinterpret_cast<uint8_t*>(Keyboard_Graphic.getDisplayBuffer());

	// nano64_t endTime = getNanoTime();
	// printfInterval(0.3, "\nTime: %5.3lfms %7.1lffps", NANO_TO_SECONDS(endTime - startTime) * 1.0e3, NANO_TO_FRAMERATE(endTime - startTime));
}

static inline constexpr uint8_t Uint8_Mult(const uint8_t value, const double mult) { return (uint8_t)((double)value * mult); }

void setRGB_Scancode(uint8_t r, uint8_t g, uint8_t b, SDL_Scancode code) {
	size_t z = (size_t)code * IMAGE_BUFFER_CHANNELS;
	Scancode_Color_Key[z] = r;
	Scancode_Color_Hover[z] = Uint8_Mult(r, 0.8333);
	Scancode_Color_Press[z] = Uint8_Mult(r, 0.75  );
	Scancode_Color_Click[z] = Uint8_Mult(r, 0.667 );
	z++;
	Scancode_Color_Key[z] = g;
	Scancode_Color_Hover[z] = Uint8_Mult(g, 0.8333);
	Scancode_Color_Press[z] = Uint8_Mult(g, 0.75  );
	Scancode_Color_Click[z] = Uint8_Mult(g, 0.667 );
	z++;
	Scancode_Color_Key[z] = b;
	Scancode_Color_Hover[z] = Uint8_Mult(b, 0.8333);
	Scancode_Color_Press[z] = Uint8_Mult(b, 0.75  );
	Scancode_Color_Click[z] = Uint8_Mult(b, 0.667 );
	z++;
	Scancode_Color_Key[z] = 0xFF;
	Scancode_Color_Hover[z] = 0xFF;
	Scancode_Color_Press[z] = 0xFF;
	Scancode_Color_Click[z] = 0xFF;
}
void setColor_Scancode(uint32_t color, SDL_Scancode code) {
	uint8_t r, g, b;
	r = color & 0xFF; color >>= 8;
	g = color & 0xFF; color >>= 8;
	b = color & 0xFF;
	setRGB_Scancode(r,g,b,code);
}
void setHSV_Scancode(double h, double s, double v, SDL_Scancode code) {
	uint8_t rK = 0, gK = 0, bK = 0;
	uint8_t rH = 0, gH = 0, bH = 0;
	uint8_t rP = 0, gP = 0, bP = 0;
	uint8_t rC = 0, gC = 0, bC = 0;
	getRGBfromHSV(&rK,&gK,&bK,h,s,v);
	getRGBfromHSV(&rH,&gH,&bH,h,s,v * 0.8333);
	getRGBfromHSV(&rP,&gP,&bP,h,s,v * 0.75);
	getRGBfromHSV(&rC,&gC,&bC,h,s,v * 0.6667);
	size_t z = (size_t)code * IMAGE_BUFFER_CHANNELS;
	Scancode_Color_Key[z] = rK;
	Scancode_Color_Hover[z] = rH;
	Scancode_Color_Press[z] = rP;
	Scancode_Color_Click[z] = rC;
	z++;
	Scancode_Color_Key[z] = gK;
	Scancode_Color_Hover[z] = gH;
	Scancode_Color_Press[z] = gP;
	Scancode_Color_Click[z] = gC;
	z++;
	Scancode_Color_Key[z] = bK;
	Scancode_Color_Hover[z] = bH;
	Scancode_Color_Press[z] = bP;
	Scancode_Color_Click[z] = bC;
	z++;
	Scancode_Color_Key[z] = 0xFF;
	Scancode_Color_Hover[z] = 0xFF;
	Scancode_Color_Press[z] = 0xFF;
	Scancode_Color_Click[z] = 0xFF;
}
