#include "SDL_keybind.hpp"

// you can edit or a new keybind to this header
#include "SDL_keybind_data.hpp"

/**
 * @brief Available Keybinds:
 * - Keybind_PortCE_Legacy
 * - Keybind_CEmu
 * - Keybind_TilEm
 * - Keybind_Wabbitemu
 * - Keybind_jsTIfied
 * - Add your own custom keybind in `PortCE/src/SDL_keybind_data.hpp`
 */
const SDL_Keybind& Default_Keybind = Keybind_PortCE_Legacy;

const SDL_Keybind& get_Default_Keybind() {
    return Default_Keybind;
}

const SDL_Keybind& get_Current_Keybind() {
    return get_Default_Keybind();
}
