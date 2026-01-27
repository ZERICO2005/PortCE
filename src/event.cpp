#include "event.hpp"
#include <SDL_events.h>
#include "PortCE_Render.h"
#include "key_input.hpp"
#include "SDL_keybind.hpp"

static void handle_key_up(const SDL_Event& event) {
    SDL_Keybind keybind = get_Current_Keybind();

    const SDL_Scancode scancode = event.key.keysym.scancode;
    for (SDL_Keybind::const_iterator it = keybind.begin(); it != keybind.end(); ++it) {
        if (it->second == scancode) {
            Key_Input_key_up(it->first);
        }
    }
}

static void handle_key_down(const SDL_Event& event) {
    SDL_Keybind keybind = get_Current_Keybind();

    const SDL_Scancode scancode = event.key.keysym.scancode;
    for (SDL_Keybind::const_iterator it = keybind.begin(); it != keybind.end(); ++it) {
        if (it->second == scancode) {
            Key_Input_key_down(it->first);
        }
    }
}

[[noreturn]] static void quit_PortCE(void) {
    terminateLCDcontroller();
    exit(0);
}

static void process_SDL_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT: { quit_PortCE(); }
        case SDL_KEYUP: {
            handle_key_up(event);
        } break;
        case SDL_KEYDOWN: {
            handle_key_down(event);
        } break;
        default: break;
    }
}

void handle_SDL_events() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        process_SDL_event(event);
    }
}
