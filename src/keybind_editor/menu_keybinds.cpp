
#include "copyBuffer.h"


#include <SDL.h>
// #include <SDL2/SDL_keycode.h>
// #include <SDL2/SDL_scancode.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

SDL_Renderer* get_imgui_SDL_renderer();

#include <cinttypes>
#include "engine.h"
#include "fracExpKB.h"

#include "keybind.h"



ImVec4 get_Theme_Highlight_Color() {
	bool useLightThemeColors = false;
	return useLightThemeColors ? ImVec4{1.0,0.0,0.0,1.0} : ImVec4{0.0,1.0,1.0,1.0};
}



template <typename fpX>
static void sen_slider(const char* label, fpX& num, float min_bound, float max_bound) {
	float temp_sensitivity_float_slider = (float)num;
	ImGui::SliderFloat(
		label, &temp_sensitivity_float_slider,
		min_bound, max_bound, "%.2f"
	);
	num = (double)temp_sensitivity_float_slider;
}

static bool ShowTheXButton = true;


void initKeys();

void init_KeyBind_PresetList();

void Menu_Keybinds() {
	static bool has_init = false;
	if (!has_init) {
		init_KeyBind_PresetList();
		initKeys();
		has_init = true;
	}
	static int Combo_keyboardSize = 0;
	static const char* keyboardSizeText[] = {
		"ANSI (Default)","Extended (Contains some FN keys)","Complete (All 242 SDL Scancodes)"
	};
	static bool displayNumpad = true;
	ImGui::Begin("Keybinds Menu",&ShowTheXButton, 0);

	ImGui::Text("Keyboard Type:");
	if (ImGui::Combo("##keyboardSize", &Combo_keyboardSize,keyboardSizeText, ARRAY_LENGTH(keyboardSizeText))) {

	}
	ImGui::Checkbox("Display Numberpad",&displayNumpad);
	if (Combo_keyboardSize != 0) {
		ImGui::Text("Note: Some Scancodes may or may not trigger on modern Keyboards");
	}
	ImGui::NewLine();
	{
        SDL_Texture* kTexture = nullptr;
		constexpr int kMaxResX = 1440;
		constexpr int kMinResX = 300;
		__attribute__((unused)) constexpr int kMinResY = 140;

		//static uint32_t kX = kMargin;
		//static uint32_t kY = 0;
		int kResX = (int)ImGui::GetContentRegionAvail().x;
		if (kResX < kMinResX) {
			kResX = kMinResX;
		} else if (kResX > kMaxResX) {
			kResX = kMaxResX;
		}
		static int32_t kCurX = -1;
		static int32_t kCurY = -1;
		static uint32_t clickState = 0x0;
		static BufferBox kBuf; // Gets set when the keyboard is rendered

		ImVec2 CursorPos = {
			ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x,
			ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - 17 // Magic Correction Amount
		};
		clickState = SDL_GetMouseState(NULL,NULL);
		static SDL_Scancode keyHover = SDL_SCANCODE_UNKNOWN;
		static SDL_Scancode keyClick = SDL_SCANCODE_UNKNOWN;
		bool hoverInBounds = false;
		renderKeyboard(
			&kBuf, kResX, 5.75, 12.0,
			(uint8_t)Combo_keyboardSize, displayNumpad,
			kCurX, kCurY, ((clickState & 0x1) ? true : false), &keyHover, nullptr, &hoverInBounds
		);
		SDL_Surface* kSurface = SDL_CreateRGBSurfaceWithFormatFrom(
			kBuf.vram,
			(int)kBuf.resX, (int)kBuf.resY,
			(int32_t)(IMAGE_BUFFER_CHANNELS * 8),
			(int32_t)(IMAGE_BUFFER_CHANNELS * (size_t)kBuf.resX),
			SDL_PIXELFORMAT_ABGR8888
		);
		if (kSurface == nullptr) {
			fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
		}
        if (kTexture) {
            FREE(kTexture);
        }
		kTexture = SDL_CreateTextureFromSurface(get_imgui_SDL_renderer(), kSurface);
		if (kTexture == nullptr) {
			fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
		}
		SDL_FreeSurface(kSurface);
		kCurX = (int32_t)CursorPos.x;
		kCurY = (int32_t)CursorPos.y;

		ImGui::Text("Hover: %s",Scancode_Name[keyHover]);
		if (clickState & 0x1 && hoverInBounds == true) {
			keyClick = keyHover;
		}
		ImGui::Image((void*)kTexture, ImVec2((float)kBuf.resX, (float)kBuf.resY));
		// ImGui::Text("size = %" PRId32 " x %" PRId32, kBuf.resX, kBuf.resY);
		// ImGui::Text("Cursor Position: %" PRId32 ",%" PRId32,kCurX,kCurY);
		ImGui::Text("Clicked Key: %s",Scancode_Name[keyClick]);
		size_t funcCount = 0;
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			using namespace Key_Function;
			for (const auto& bind : currentKBPreset->kList) {
				if (bind.key == keyClick) {
					funcCount++;
				}
			}
			if (funcCount > 0) {
				//ImGui::Text("Key Functions:");
				for (const auto& bind : currentKBPreset->kList) {
					if (bind.key == keyClick) {
						ImGui::Text("- %s",Key_Function_Text[bind.func]);
					}
				}
			} else {
				ImGui::Text("- Key not bound to any functions");
			}
		} else {
			ImGui::NewLine();
		}

		ImGui::Separator();

		static int Combo_function_Select = 0;
		// static int Combo_function_category_Select = 0;
		// ImGui::Text("Select Function Category:");
		// if (ImGui::Combo("##Combo_function_category_Select",&Combo_function_category_Select,
		// 	Key_Function::Key_Function_Category_Text, ARRAY_LENGTH(Key_Function::Key_Function_Category_Text)
		// )) {

		// }

		//if (Combo_function_category_Select != Key_Function::Category_NONE) {
			// size_t key_func_offset = Key_Function::Key_Function_Map[Combo_function_category_Select];
			// size_t key_func_previous = Key_Function::Key_Function_Map[Combo_function_category_Select - 1];
			// const char* key_func_start = Key_Function::Key_Function_Text[key_func_previous];
			// const char* key_func_end = Key_Function::Key_Function_Text[key_func_offset];
			// size_t key_func_length = key_func_end - key_func_start;
			// ImGui::Text("Select Function to bind:");
			// if (ImGui::Combo("##Combo_functionSelect",&Combo_function_Select,
			// 	key_func_start, key_func_length
			// )) {

			ImGui::Text("Select Function to bind:");
			if (ImGui::Combo("##Combo_functionSelect",&Combo_function_Select,
					Key_Function::Key_Function_Text, ARRAY_LENGTH(Key_Function::Key_Function_Text)
			)) {
				if (Combo_function_Select == Key_Function::Parameter_Function_Count) {
					Combo_function_Select = Key_Function::NONE;
				} else {
					for (size_t i = 0; i < ARRAY_LENGTH(Key_Function::Key_Function_Map); i++) {
						if (Combo_function_Select == (int_enum)Key_Function::Key_Function_Map[i]) {
							Combo_function_Select = Key_Function::NONE;
						}
					}
				}
			}
		//}

		ImVec4 bind_select_color = get_Theme_Highlight_Color();

		if (Combo_function_Select != Key_Function::NONE && keyClick != SDL_SCANCODE_UNKNOWN) {
			//ImGui::Text("Bind key %s to function %s",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);

			ImGui::Text("Bind key "); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored(bind_select_color,"%s",Scancode_Name[keyClick]); ImGui::SameLine(0.0,1.0);
			ImGui::Text(" to function "); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored(bind_select_color,"%s",Key_Function::Key_Function_Text[Combo_function_Select]);

			if(ImGui::Button("Set Key-Bind")) {
				if (addKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_function_Select,keyClick) >= 0) {
					recolorKeyboard();
					Combo_function_Select = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					fprintf(stderr, "addKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_function_Select]);
				}
			}
			if (funcCount != 0) {
				ImGui::SameLine();
				if(ImGui::Button("Clear Key-bind")) {
					if (removeKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_function_Select,keyClick) >= 0) {
						recolorKeyboard();
						Combo_function_Select = Key_Function::NONE;
					} else {
						fprintf(stderr, "removeKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_function_Select]);
					}
				}
			}
		} else {
			ImGui::Text("Click on a Key and Select a Function");
			ImGui::NewLine();
		}
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			if(ImGui::Button("Clear Key bindings")) {
				if (removeKeyBind(&currentKBPreset->kList,keyClick) >= 0) {
					recolorKeyboard();
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					fprintf(stderr, "removeKeyBind(%s) failed",Scancode_Name[keyClick]);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s",Scancode_Name[keyClick]);
		} else {
			ImGui::Button("Click on a Key");
		}
		if (Combo_function_Select != Key_Function::NONE) {
			if(ImGui::Button("Clear Function bindings")) {
				if (removeKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_function_Select) >= 0) {
					recolorKeyboard();
				} else {
					fprintf(stderr, "removeKeyBind(%s) failed",Key_Function::Key_Function_Text[Combo_function_Select]);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s",Key_Function::Key_Function_Text[Combo_function_Select]);
		} else {
			ImGui::Button("Select a Function");
		}
		ImGui::NewLine();
		if (ImGui::Button("Reset current key-bind to defaults")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			currentKBPreset->kList = defaultKeyBind;
			recolorKeyboard();
		}
		ImGui::NewLine();

		ImGui::Separator();

		static char KeyBindName[80];
		memset(KeyBindName,'\0',ARRAY_LENGTH(KeyBindName));
		memcpy(KeyBindName,currentKBPreset->name.c_str(),ARRAY_LENGTH(KeyBindName) - 1);
		if (currentKBPreset->kList.size() < 6) {
			ImGui::Text("Warning: The current Key-bind Preset has %zu key-binds, and may not be functional or practical.",currentKBPreset->kList.size());
			ImGui::Text("Current Key-bind Preset[%zu]: ",(size_t)get_currentKBPreset_Pos()); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored({1.0,0.5,0.5,1.0},"%zu",currentKBPreset->kList.size()); ImGui::SameLine(0.0,1.0);
			ImGui::Text(" key-binds");
		} else {
			ImGui::Text("Current Key-bind Preset[%" PRId32 "]: %" PRIu64 " key-binds",get_currentKBPreset_Pos(),currentKBPreset->kList.size());
		}

		ImGui::InputText("##KeyBindName",(KeyBindName), ARRAY_LENGTH(KeyBindName));
		currentKBPreset->name = KeyBindName;
		if (ImGui::BeginCombo("##Combo_KeyBind", "Choose a key-bind")) {
			for (auto iterKB = KeyBind_PresetList.begin(); iterKB != KeyBind_PresetList.end(); iterKB++) {
				bool is_selected = (currentKBPreset == &(*iterKB));
				if (ImGui::Selectable(iterKB->name.c_str(), is_selected)) {
					Combo_function_Select = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
					currentKBPreset = &(*iterKB);
					recolorKeyboard();
				}
				// Set the initial focus on the currently selected item
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::NewLine();
		static uint32_t name_count = 1;
		if (ImGui::Button("Create key-bind")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			KeyBind_Preset temp_KeyBind;
			temp_KeyBind = *currentKBPreset;
			static char rand_name[324]; memset(rand_name,'\0',324);
			snprintf(rand_name,320,"KeyBind_%" PRIu32,name_count++);
			temp_KeyBind.name = rand_name;
			KeyBind_PresetList.push_back(temp_KeyBind);
			currentKBPreset = &KeyBind_PresetList.back();
			recolorKeyboard();
		}
		if (KeyBind_PresetList.size() > 1 && ImGui::Button("Remove current key-bind preset")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			remove_currentKBPreset();
			recolorKeyboard();
		}
		if (ImGui::Button("Clear all key-binds")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			clearKeyBind(&currentKBPreset->kList);
			recolorKeyboard();
		}

		ImGui::NewLine();

	ImGui::End();
    }
}
