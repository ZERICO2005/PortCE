#include "fracExpKB.h"
#include "../PortCE_Common.h"
#include "keybind.h"
#include <list>
#include <algorithm>
#include "engine.h"
#include <SDL_keycode.h>
#include <SDL_scancode.h>
#include <SDL.h>

size_t KeyBind_PresetCount = 1;
std::list<KeyBind_Preset> KeyBind_PresetList;
KeyBind_Preset* currentKBPreset = nullptr;

struct Function_Status {
	bool triggered;
	nano64_t resetTime;
	nano64_t freqTime;
};

Function_Status func_stat[Key_Function::Parameter_Function_Count];

const uint8_t* SDL_Keyboard_State;

struct Key_Status {
    SDL_Scancode key;
	bool pressed;
	uint64_t timePressed;
	uint64_t timeReleased;
}; typedef struct Key_Status Key_Status;
Key_Status Key_List[SDL_NUM_SCANCODES];

void init_KeyBind_PresetList() {
		if (KeyBind_PresetList.empty() == true) {
			KeyBind_Preset temp_KeyBind;
			initDefaultKeyBind(&temp_KeyBind.kList);
			temp_KeyBind.name = "Default";
			KeyBind_PresetList.push_front(temp_KeyBind);
			currentKBPreset = &KeyBind_PresetList.front();
		}
	}
	void clear_KeyBind_PresetList() {
		KeyBind_PresetList.clear();
	}
	int32_t get_currentKBPreset_Pos() {
		std::size_t index = 0;
		for (const auto& element : KeyBind_PresetList) {
			if (&element == currentKBPreset) {
				return (int32_t)index;
			}
			index++;
		}
		return -1;
	}
	void set_currentKBPreset_Pos(int pos) {
		if (pos < 0 || pos >= (int)KeyBind_PresetList.size()) {
			return;
		}
		std::list<KeyBind_Preset>::iterator iterKBP = KeyBind_PresetList.begin();
		std::advance(iterKBP, pos);
		if (iterKBP != KeyBind_PresetList.end()) {
			currentKBPreset = &(*iterKBP);
		}
	}
	void remove_currentKBPreset() {
		if (KeyBind_PresetList.size() <= 1) {
			return;
		}
		int pos = 0;
		for (std::list<KeyBind_Preset>::iterator iterKBP = KeyBind_PresetList.begin(); iterKBP != KeyBind_PresetList.end(); iterKBP++) {
			if (currentKBPreset == &(*iterKBP)) {
				KeyBind_PresetList.erase(iterKBP);
				if (pos == 0) {
					currentKBPreset = &KeyBind_PresetList.front();
				} else {
					set_currentKBPreset_Pos(pos - 1);
				}
				return;
			}
			pos++;
		}
	}
	const char* currentKBPreset_Name() {
		return currentKBPreset->name.c_str();
	}
	std::list<KeyBind> currentKeyBind() {
		return currentKBPreset->kList;
	}

// Key_Status Key_List[SDL_NUM_SCANCODES];

// Function_Status func_stat[Key_Function::Parameter_Function_Count];

void updateKeys() {
	for (size_t t = 0; t < ARRAY_LENGTH(func_stat); t++) {
		func_stat[t].triggered = false;
	}
	SDL_Keyboard_State = SDL_GetKeyboardState(NULL);
	for (size_t i = 0; i < SDL_NUM_SCANCODES; i++) {
		if (SDL_Keyboard_State[i] != 0) { // Key Pressed
			if (Key_List[i].pressed == false) {
				Key_List[i].timePressed = getNanoTime();
				Key_List[i].pressed = true;
			}
		} else {
			Key_List[i].timeReleased = getNanoTime();
			Key_List[i].pressed = false;
		}

		for (const auto& bind : currentKBPreset->kList) {
			if (bind.key == (SDL_Scancode)i) {
				Key_Function::Key_Function_Enum func = bind.func;
                bool Lock_Key_Inputs = false;
				if (Lock_Key_Inputs == true) { // Only listens to FUNCTION key-binds when key inputs are locked
					// if (func <= Key_Function::FORMULA /* Key_Function::SCREEN_SPLIT */ || func >= Key_Function::FUNCTIONS) {
					// 	continue;
					// }
				}
				if (Key_List[i].pressed == true) {
						func_stat[func].triggered = true;
				}
			}
		}
	}
	//write_Function_Status(func_stat);
}

void recolorKeyboard() {
	{
		using namespace Key_Function;
		initKeyboardGraphics(0.0,0.0,0.5);
		struct init_key_HSV {
			enum Key_Function_Enum type;
			double h; double s; double v;
		};
		init_key_HSV InitKeyHSV[] = {
			{NONE,0.0,0.0,0.5},
			{PORTCE,0.0,0.7,1.0},{DIRECTION,180.0,0.7,1.0},{ACTION,300.0,0.6,1.0},
			{TOP_ROW,59.9,1.0,1.0},{NUMBER,240.0,0.7,0.7},{OTHER,210.0,0.3,0.8},
			/* {SCREEN_SPLIT,30.0,0.3,1.0}, */ {POWER,120.0,0.8,0.8}, /* {RENDERING,180.0,0.4,0.6}, */
		};
		struct init_key_RGB {
			enum Key_Function_Enum type;
			uint8_t r; uint8_t g; uint8_t b;
		};
		init_key_RGB InitKeyRGB[ARRAY_LENGTH(InitKeyHSV)];
		for (size_t i = 0; i < ARRAY_LENGTH(InitKeyHSV); i++) {
			getRGBfromHSV(&(InitKeyRGB[i].r),&(InitKeyRGB[i].g),&(InitKeyRGB[i].b),InitKeyHSV[i].h,InitKeyHSV[i].s,InitKeyHSV[i].v);
		}
		// if (currentKBPreset->kList.size() <= 0) {
		// 	currentKBPreset->kList = defaultKeyBind;
		// }
		for (size_t s = SDL_SCANCODE_UNKNOWN + 1; s < SDL_NUM_SCANCODES; s++) {
			size_t keyColorSet = 0;
			for (const auto& bind : currentKBPreset->kList) {
				if (bind.key == (SDL_Scancode)s) {
					if (keyColorSet != 0) { // Detects if different function types are binded to a key
						setRGB_Scancode(0xFF,0xFF,0xFF,(SDL_Scancode)s);
						break; // Idempotent
					}
					for (size_t r = Key_Function::NONE + 1; r < ARRAY_LENGTH(Key_Function_Map); r++) {
						if (bind.func <= Key_Function_Map[r] && bind.func != Key_Function::NONE) {
							setRGB_Scancode(InitKeyRGB[r].r,InitKeyRGB[r].g,InitKeyRGB[r].b,(SDL_Scancode)s);
							keyColorSet = r;
							break;
						}
					}
				}
			}
		}
	}
}

void initKeys() {
	recolorKeyboard();
	for (size_t i = 0; i < SDL_NUM_SCANCODES; i++) {
		Key_List[i].timePressed = getNanoTime();
		Key_List[i].key = (SDL_Scancode)i;
		Key_List[i].pressed = false;
		Key_List[i].timeReleased = getNanoTime();
	}
	updateKeys();
}

bool keyPressed(SDL_Scancode key) {
	if (key > SDL_NUM_SCANCODES) {
		return false;
	}
	if (Key_List[key].pressed == true) {
		return true;
	}
	return false;
}
