/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "fracExp_Internal.h"
#include "fracExpKB.h"

/* Key-bind List Functions */
	// Compares Key-binds for sorting purposes
	bool compareKeyBind(const KeyBind& a, const KeyBind& b) {
		if (a.key != b.key) {
			return a.key < b.key;
		}
		return a.func < b.func;
	}
	bool equalsKeyBind(const KeyBind& a, const KeyBind& b) {
		return a.key == b.key && a.func == b.func;
	}
	// Resets a Key-bind to defaults
	int initDefaultKeyBind(std::list<KeyBind>* keyBindList) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->clear();
		*keyBindList = defaultKeyBind;
		return 0;
	}
	// Clears a Key-bind
	int clearKeyBind(std::list<KeyBind>* keyBindList) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->clear();
		return 0;
	}
	// Add a Key-bind
	int addKeyBind(std::list<KeyBind>* keyBindList, Key_Function::Key_Function_Enum func, SDL_Scancode key) {
		if (keyBindList == nullptr) { return -1; }
		if (func == Key_Function::NONE && key == SDL_SCANCODE_UNKNOWN) {
			return -1;
		}
		KeyBind k; k.func = func; k.key = key;
		// if (std::find(keyBindList->begin(),keyBindList->end(),k) != keyBindList->end()) {
		// 	return 1;
		// }
		keyBindList->push_back(k);
		return 0;
	}
	// Removes a specific Key-bind
	int removeKeyBind(std::list<KeyBind>* keyBindList, Key_Function::Key_Function_Enum func, SDL_Scancode key) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([&func,&key](const KeyBind& kb) {
			return kb.func == func && kb.key == key;
		});
		return 0;
	}
	// Removes any Key-bind matching a Function
	int removeKeyBind(std::list<KeyBind>* keyBindList, Key_Function::Key_Function_Enum func) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([&func](const KeyBind& kb) {
			return kb.func == func;
		});
		return 0;
	}
	// Removes any Key-bind matching a SDL_Scancode
	int removeKeyBind(std::list<KeyBind>* keyBindList, SDL_Scancode key) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([&key](const KeyBind& kb) {
			return kb.key == key;
		});
		return 0;
	}
	// Removes duplicated Key-binds and any Key-binds with nothing set
	int cleanKeyBind(std::list<KeyBind>* keyBindList) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([](const KeyBind& kb) {
			return kb.key == 0 && kb.func == 0;
		});
		keyBindList->sort(compareKeyBind);
		keyBindList->unique(equalsKeyBind);
		return 0;
	}



int create_FracExpKB_Preset(FracExpKB_File* frac, const char* fracExpKB_raw, const Param_List* param_list, size_t param_len);

/* Import/Read FracExpKB */

/* Export/Write FracExpKB */
