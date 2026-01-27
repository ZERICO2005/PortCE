#ifndef SDL_KEYBIND_HPP
#define SDL_KEYBIND_HPP

#include <utility>
#include <vector>
#include "key_input.hpp"
#include <SDL_scancode.h>

typedef std::vector<std::pair<Key_Input, SDL_Scancode>> SDL_Keybind;

const SDL_Keybind& get_Current_Keybind();

const SDL_Keybind& get_Default_Keybind();

#endif /* SDL_KEYBIND_HPP */
