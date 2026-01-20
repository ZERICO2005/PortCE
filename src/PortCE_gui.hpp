#ifndef PORTCE_GUI_HPP
#define PORTCE_GUI_HPP

#include <SDL_events.h>

void begin_PortCE_IMGUI(void);

void finish_PortCE_IMGUI(void);

void render_IMGUI(void);

void PortCE_set_imgui_visible(bool visible);
bool PortCE_is_imgui_visible(void);
void PortCE_process_imgui_event(const SDL_Event* event);
bool PortCE_is_imgui_window_id(uint32_t window_id);

#endif /* PORTCE_GUI_HPP */
